#pragma once
#include "parser.hpp"
#include "statement.hpp"
#include "utils.hpp"
#include <algorithm>
#include <list>
#include <string>
#include <vector>

class Table {
public:
  Table(const std::string &name, const std::vector<ColumnDefinition> &columns)
      : name(name), columns(columns) {
    // Build column index
    for (size_t i = 0; i < columns.size(); i++) {
      column_index[columns[i].name] = i;
    }
  }

  void insert(const std::vector<Value> &row) {
    // Validate number of values matches number of columns
    if (row.size() != columns.size()) {
      throw TableError("Number of values does not match number of columns");
    }

    // Validate that each value matches its column's type
    for (size_t i = 0; i < row.size(); i++) {
      const auto &value = row[i];
      bool ok = (std::holds_alternative<int>(value) &&
                 columns[i].type == TokenType::INTEGER) ||
                (std::holds_alternative<double>(value) &&
                 columns[i].type == TokenType::FLOAT) ||
                (std::holds_alternative<std::string>(value) &&
                 columns[i].type == TokenType::TEXT);
      if (!ok) {
        std::cerr << "Value: ";
        std::visit([](const auto &v) { std::cerr << v; }, value);
        std::cerr << " Type: " << TOKEN_STR.find(columns[i].type)->second
                  << ' ';
        throw TableError("Value does not match column type");
      }
    }

    // Store the row values in the table data
    rows.push_back(row);
  }

  void select(const SelectStatement &stmt) {
    // Find rows matching where condition
    std::vector<Value> column_names;
    if (stmt.columns.empty()) {
      for (const auto &column : columns) {
        column_names.push_back(Value(column.name));
      }
    } else {
      for (const std::string &column_name : stmt.columns) {
        column_names.push_back(Value(column_name));
      }
    }
    std::vector<std::vector<Value>> results({column_names});
    for (const auto &row : rows) {
      bool matches = true;
      if (stmt.where_condition) {
        matches = compareValues(
            stmt.where_condition->column_name_a,
            stmt.where_condition->condition_type_a,
            convertTokenToValue(stmt.where_condition->value_a), row);
        if (stmt.where_condition->logic_operator == TokenType::AND) {
          matches =
              matches &&
              compareValues(stmt.where_condition->column_name_b,
                            stmt.where_condition->condition_type_b,
                            convertTokenToValue(stmt.where_condition->value_b),
                            row);
        } else if (stmt.where_condition->logic_operator == TokenType::OR) {
          matches =
              matches ||
              compareValues(stmt.where_condition->column_name_b,
                            stmt.where_condition->condition_type_b,
                            convertTokenToValue(stmt.where_condition->value_b),
                            row);
        }
      }
      if (matches) {
        if (stmt.columns.empty()) {
          // Select all columns
          results.push_back(row);
        } else {
          // Select specific columns
          std::vector<Value> selected;
          for (const auto &colName : stmt.columns) {
            auto it = std::find_if(columns.begin(), columns.end(),
                                   [&colName](const ColumnDefinition &col) {
                                     return col.name == colName;
                                   });
            if (it != columns.end()) {
              size_t index = std::distance(columns.begin(), it);
              selected.push_back(row[index]);
            } else {
              throw TableError("Column not found");
            }
          }
          if (!selected.empty())
            results.push_back(selected);
        }
      }
    }

    file_writer.write(results);
  }

  void update(const UpdateStatement &stmt) {
    // Find rows matching where condition and update them
    for (auto &row : rows) {
      bool matches = true;
      if (stmt.where_condition) {
        matches = compareValues(
            stmt.where_condition->column_name_a,
            stmt.where_condition->condition_type_a,
            convertTokenToValue(stmt.where_condition->value_a), row);
        if (stmt.where_condition->logic_operator == TokenType::AND) {
          matches =
              matches &&
              compareValues(stmt.where_condition->column_name_b,
                            stmt.where_condition->condition_type_b,
                            convertTokenToValue(stmt.where_condition->value_b),
                            row);
        } else if (stmt.where_condition->logic_operator == TokenType::OR) {
          matches =
              matches ||
              compareValues(stmt.where_condition->column_name_b,
                            stmt.where_condition->condition_type_b,
                            convertTokenToValue(stmt.where_condition->value_b),
                            row);
        }
      }

      if (matches) {
        // Update matching rows with new values
        for (const auto &set_condition : stmt.set_conditions) {
          // Find column index
          auto it = column_index.find(set_condition.column_name_a);
          if (it == column_index.end()) {
            throw TableError("Column not found: " +
                             set_condition.column_name_a);
          }
          size_t index = it->second;

          // Validate that the new value matches the column type
          if (set_condition.column_name_b.empty()) {
            const auto &column_type = columns[index].type;
            auto value = convertTokenToValue(set_condition.value);
            bool type_ok = (std::holds_alternative<int>(value) &&
                            column_type == TokenType::INTEGER) ||
                           (std::holds_alternative<double>(value) &&
                            column_type == TokenType::FLOAT) ||
                           (std::holds_alternative<std::string>(value) &&
                            column_type == TokenType::TEXT);

            if (!type_ok) {
              std::cerr << "Value: ";
              std::visit([](const auto &v) { std::cerr << v; }, value);
              std::cerr << " Type: " << TOKEN_STR.find(column_type)->second
                        << ' ';

              std::cerr << " Type: "
                        << TOKEN_STR.find(set_condition.value.type)->second
                        << ' ';
              std::cerr << " Value: " << set_condition.value.value << ' ';
              throw TableError("Value type does not match column type");
            }

            // Update the value
            row[index] = value;
          } else {
            // Find the source column index
            auto src_it = column_index.find(set_condition.column_name_b);
            if (src_it == column_index.end()) {
              throw TableError("Source column not found: " +
                               set_condition.column_name_b);
            }
            size_t src_index = src_it->second;

            // Get the value from the source column
            const auto &src_value = row[src_index];

            // Apply the operator (+ or -) if specified
            if (set_condition.operator_type == TokenType::PLUS) {
              // Handle addition
              if (std::holds_alternative<int>(src_value) &&
                  std::holds_alternative<int>(
                      convertTokenToValue(set_condition.value))) {
                row[index] =
                    std::get<int>(src_value) +
                    std::get<int>(convertTokenToValue(set_condition.value));
              } else if (std::holds_alternative<double>(src_value) &&
                         std::holds_alternative<double>(
                             convertTokenToValue(set_condition.value))) {
                row[index] =
                    std::get<double>(src_value) +
                    std::get<double>(convertTokenToValue(set_condition.value));
              } else {
                throw TableError("Invalid types for addition operation");
              }
            } else if (set_condition.operator_type == TokenType::MINUS) {
              // Handle subtraction
              if (std::holds_alternative<int>(src_value) &&
                  std::holds_alternative<int>(
                      convertTokenToValue(set_condition.value))) {
                row[index] =
                    std::get<int>(src_value) -
                    std::get<int>(convertTokenToValue(set_condition.value));
              } else if (std::holds_alternative<double>(src_value) &&
                         std::holds_alternative<double>(
                             convertTokenToValue(set_condition.value))) {
                row[index] =
                    std::get<double>(src_value) -
                    std::get<double>(convertTokenToValue(set_condition.value));
              } else {
                throw TableError("Invalid types for subtraction operation");
              }
            } else {
              throw TableError("Invalid operator type");
            }
          }
        }
      }
    }
  }

  void deleteRows(const DeleteStatement &stmt) {
    // If there's no where condition, delete all rows
    if (!stmt.where_condition) {
      rows.clear();
      return;
    }

    // Iterate through rows and remove those that match the where condition
    auto it = rows.begin();
    while (it != rows.end()) {
      bool should_delete = true;

      // Check first condition
      should_delete = compareValues(
          stmt.where_condition->column_name_a,
          stmt.where_condition->condition_type_a,
          convertTokenToValue(stmt.where_condition->value_a), *it);

      // If there's a second condition, check it too
      if (stmt.where_condition->logic_operator != TokenType::EOF_TOKEN) {
        bool second_condition = compareValues(
            stmt.where_condition->column_name_b,
            stmt.where_condition->condition_type_b,
            convertTokenToValue(stmt.where_condition->value_b), *it);

        // Combine conditions based on logic operator
        if (stmt.where_condition->logic_operator == TokenType::AND) {
          should_delete = should_delete && second_condition;
        } else if (stmt.where_condition->logic_operator == TokenType::OR) {
          should_delete = should_delete || second_condition;
        }
      }

      // Remove the row if it matches the condition(s)
      if (should_delete) {
        it = rows.erase(it);
      } else {
        ++it;
      }
    }
  }

  void innerJoin(const InnerJoinStatement &stmt, Table &table_b) {
    size_t col_idx_a = column_index[stmt.column_name_a];
    size_t col_idx_b = table_b.column_index[stmt.column_name_b];

    std::vector<std::vector<Value>> results({std::vector<Value>(
        {Value(stmt.column_name_a), Value(stmt.column_name_b)})});

    for (const auto &row_a : rows) {
      const Value &value_a = row_a[col_idx_a];
      for (const auto &row_b : table_b.rows) {
        const Value &value_b = row_b[col_idx_b];

        if (value_a == value_b) {
          std::vector<Value> combined_row;
          for (const auto &val : row_a) {
            combined_row.push_back(val);
          }
          for (const auto &val : row_b) {
            combined_row.push_back(val);
          }

          results.push_back(combined_row);
        }
      }
    }

    file_writer.write(results);
  }

private:
  std::string name;
  std::vector<ColumnDefinition> columns;
  std::list<std::vector<Value>> rows;
  std::unordered_map<std::string, size_t> column_index;

  bool compareValues(const std::string &column_name, TokenType condition_type,
                     const Value &value, const std::vector<Value> &row) {
    // Find column index
    auto it = column_index.find(column_name);
    if (it == column_index.end()) {
      throw TableError("Column not found");
    }
    size_t index = it->second;

    // Compare value with row value
    const auto &row_value = row[index];
    if (std::holds_alternative<int>(value) &&
        std::holds_alternative<int>(row_value)) {
      int val = std::get<int>(value);
      int row_val = std::get<int>(row_value);
      switch (condition_type) {
      case TokenType::EQUALS:
        return row_val == val;
      case TokenType::LESS_THAN:
        return row_val < val;
      case TokenType::GREATER_THAN:
        return row_val > val;
      default:
        throw TableError("Invalid condition type");
      }
    } else if (std::holds_alternative<double>(value) &&
               std::holds_alternative<double>(row_value)) {
      double val = std::get<double>(value);
      double row_val = std::get<double>(row_value);
      switch (condition_type) {
      case TokenType::EQUALS:
        return row_val == val;
      case TokenType::LESS_THAN:
        return row_val < val;
      case TokenType::GREATER_THAN:
        return row_val > val;
      default:
        throw TableError("Invalid condition type");
      }
    } else if (std::holds_alternative<std::string>(value) &&
               std::holds_alternative<std::string>(row_value)) {
      std::string val = std::get<std::string>(value);
      std::string row_val = std::get<std::string>(row_value);
      switch (condition_type) {
      case TokenType::EQUALS:
        return row_val == val;
      default:
        throw TableError("Invalid condition type");
      }
    } else {
      throw TableError("Value types do not match");
    }
  }
};