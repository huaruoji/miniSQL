#pragma once
#include "parser.hpp"
#include "statement.hpp"
#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

class Table {
public:
  Table(const std::string &name, const std::vector<ColumnDefinition> &columns)
      : name(name), columns(columns) {
    rebuildColumnIndex();
  }

  void rebuildColumnIndex() {
    column_index.clear();
    for (size_t i = 0; i < columns.size(); i++) {
      column_index[columns[i].name] = i;
    }
  }

  std::string getName() const { return name; }

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
          auto it = column_index.find(set_condition.target_column);
          if (it == column_index.end()) {
            throw TableError("Column not found: " + set_condition.target_column);
          }
          size_t index = it->second;

          // Evaluate the expression and update the value
          Value new_value = evaluateExpression(set_condition.expression.get(), row);
          
          // Validate that the new value matches the column type
          const auto &column_type = columns[index].type;
          bool type_ok = (std::holds_alternative<int>(new_value) && column_type == TokenType::INTEGER) ||
                        (std::holds_alternative<double>(new_value) && column_type == TokenType::FLOAT) ||
                        (std::holds_alternative<std::string>(new_value) && column_type == TokenType::TEXT);

          if (!type_ok) {
            throw TableError("Value type does not match column type");
          }

          // Update the value
          row[index] = new_value;
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

  void innerJoin(const InnerJoinStatement &stmt, std::vector<std::unique_ptr<Table>> &other_tables) {
    // Map to store table name to index mapping
    std::unordered_map<std::string, size_t> table_indices;
    table_indices[name] = 0;  // Current table is at index 0
    for (size_t i = 0; i < other_tables.size(); ++i) {
      table_indices[other_tables[i]->getName()] = i + 1;
    }

    // Vector to store all tables (current table + other tables)
    std::vector<Table*> all_tables;
    all_tables.push_back(this);
    for (const auto& table : other_tables) {
      all_tables.push_back(table.get());
    }

    // Function to get table and column index from a qualified name (table.column)
    auto getTableColumnIndex = [&](const std::string& qualified_name) -> std::pair<Table*, size_t> {
      size_t dot_pos = qualified_name.find('.');
      if (dot_pos == std::string::npos) {
        throw TableError("Column name must be qualified with table name: " + qualified_name);
      }
      
      std::string table_name = qualified_name.substr(0, dot_pos);
      std::string column_name = qualified_name.substr(dot_pos + 1);
      
      auto table_idx_it = table_indices.find(table_name);
      if (table_idx_it == table_indices.end()) {
        throw TableError("Table not found: " + table_name);
      }
      
      Table* table = all_tables[table_idx_it->second];
      
      auto col_idx_it = table->column_index.find(column_name);
      if (col_idx_it == table->column_index.end()) {
        throw TableError("Column not found: " + column_name + " in table " + table_name);
      }
      
      return {table, col_idx_it->second};
    };

    // Initialize results with column names
    std::vector<std::vector<Value>> results;
    std::vector<Value> header;
    for (const auto& col : stmt.selected_columns) {
      header.push_back(Value(col));
    }
    results.push_back(header);

    // Helper function to get column value from a row
    auto getColumnValue = [](const std::vector<Value>& row, size_t col_idx) -> const Value& {
      if (col_idx >= row.size()) {
        throw TableError("Column index out of range: " + std::to_string(col_idx));
      }
      return row[col_idx];
    };

    // Helper function to check join condition
    auto checkJoinCondition = [](const Value& val1, const Value& val2, TokenType op) -> bool {
      switch (op) {
        case TokenType::EQUALS:
          return val1 == val2;
        case TokenType::GREATER_THAN:
          return val1 > val2;
        case TokenType::LESS_THAN:
          return val1 < val2;
        case TokenType::INEQUALS:
          return val1 != val2;
        default:
          throw TableError("Unsupported operator in join condition");
      }
    };

    // Start with rows from the first table
    std::vector<std::vector<Value>> current_results;
    for (const auto& row : rows) {
      current_results.push_back(row);
    }

    // For each join condition
    for (size_t i = 0; i < stmt.join_conditions.size(); ++i) {
      const auto& condition = stmt.join_conditions[i];
      TokenType op = stmt.join_operators[i];
      
      auto [table_a, col_idx_a] = getTableColumnIndex(condition.first);
      auto [table_b, col_idx_b] = getTableColumnIndex(condition.second);
      
      std::vector<std::vector<Value>> new_results;
      
      // For each row in current results
      for (const auto& current_row : current_results) {
        // For each row in the table we're joining with
        for (const auto& row_b : table_b->rows) {
          // Get the actual values for comparison
          Value val_a;
          if (table_a == this) {
            // Value is in the current row
            val_a = current_row[col_idx_a];
          } else if (table_a == table_b) {
            // Value is in row_b
            val_a = row_b[col_idx_a];
          } else {
            // Value must be from a previously joined table
            size_t offset = 0;
            for (const auto& table : all_tables) {
              if (table == table_a) {
                val_a = current_row[offset + col_idx_a];
                break;
              }
              offset += table->columns.size();
            }
          }

          Value val_b;
          if (table_b == this) {
            // Value is in the current row
            val_b = current_row[col_idx_b];
          } else {
            // Value is in row_b
            val_b = row_b[col_idx_b];
          }
          
          if (checkJoinCondition(val_a, val_b, op)) {
            // Combine the rows
            std::vector<Value> combined_row = current_row;
            combined_row.insert(combined_row.end(), row_b.begin(), row_b.end());
            new_results.push_back(combined_row);
          }
        }
      }
      
      current_results = std::move(new_results);
    }

    // Apply WHERE condition if present
    if (stmt.where_condition) {
      std::vector<std::vector<Value>> filtered_results;
      for (const auto& row : current_results) {
        bool matches = true;
        
        // Get values for the condition
        auto [table_a, col_idx_a] = getTableColumnIndex(stmt.where_condition->column_name_a);
        Value val_a;
        
        // Calculate offset for table_a
        size_t offset_a = 0;
        for (const auto& table : all_tables) {
          if (table == table_a) {
            break;
          }
          offset_a += table->columns.size();
        }
        val_a = row[offset_a + col_idx_a];
        
        // Get the second value
        Value val_b;
        if (stmt.where_condition->value_a.type == TokenType::IDENTIFIER) {
          // Value is a column reference
          auto [table_b, col_idx_b] = getTableColumnIndex(stmt.where_condition->value_a.value);
          // Calculate offset for table_b
          size_t offset_b = 0;
          for (const auto& table : all_tables) {
            if (table == table_b) {
              break;
            }
            offset_b += table->columns.size();
          }
          val_b = row[offset_b + col_idx_b];
        } else {
          // Value is a literal
          val_b = convertTokenToValue(stmt.where_condition->value_a);
        }
        
        matches = checkJoinCondition(val_a, val_b, stmt.where_condition->condition_type_a);
        
        if (stmt.where_condition->logic_operator != TokenType::EOF_TOKEN) {
          auto [table_c, col_idx_c] = getTableColumnIndex(stmt.where_condition->column_name_b);
          // Calculate offset for table_c
          size_t offset_c = 0;
          for (const auto& table : all_tables) {
            if (table == table_c) {
              break;
            }
            offset_c += table->columns.size();
          }
          Value val_c = row[offset_c + col_idx_c];
          
          // Get the second value
          Value val_d;
          if (stmt.where_condition->value_b.type == TokenType::IDENTIFIER) {
            // Value is a column reference
            auto [table_d, col_idx_d] = getTableColumnIndex(stmt.where_condition->value_b.value);
            // Calculate offset for table_d
            size_t offset_d = 0;
            for (const auto& table : all_tables) {
              if (table == table_d) {
                break;
              }
              offset_d += table->columns.size();
            }
            val_d = row[offset_d + col_idx_d];
          } else {
            // Value is a literal
            val_d = convertTokenToValue(stmt.where_condition->value_b);
          }
          
          bool second_condition = checkJoinCondition(val_c, val_d, 
                                                  stmt.where_condition->condition_type_b);
          
          if (stmt.where_condition->logic_operator == TokenType::AND) {
            matches = matches && second_condition;
          } else if (stmt.where_condition->logic_operator == TokenType::OR) {
            matches = matches || second_condition;
          }
        }
        
        if (matches) {
          filtered_results.push_back(row);
        }
      }
      current_results = std::move(filtered_results);
    }

    // Select only the requested columns
    for (const auto& row : current_results) {
      std::vector<Value> selected_row;
      for (const auto& col : stmt.selected_columns) {
        auto [table, col_idx] = getTableColumnIndex(col);
        
        // Calculate offset for the table
        size_t offset = 0;
        for (const auto& t : all_tables) {
          if (t == table) {
            break;
          }
          offset += t->columns.size();
        }
        
        // Get the value from the correct position
        size_t actual_idx = offset + col_idx;
        if (actual_idx >= row.size()) {
          throw TableError("Column index out of range: " + std::to_string(actual_idx));
        }
        selected_row.push_back(row[actual_idx]);
      }
      results.push_back(selected_row);
    }

    file_writer.write(results);
  }

  // Evaluate an expression node recursively
  Value evaluateExpression(const ExpressionNode* node, const std::vector<Value>& row) {
    switch (node->type) {
      case ExprNodeType::VALUE: {
        if (node->token.type == TokenType::IDENTIFIER) {
          // Get value from column
          auto it = column_index.find(node->token.value);
          if (it == column_index.end()) {
            throw TableError("Column not found: " + node->token.value);
          }
          return row[it->second];
        } else {
          // Return literal value
          return convertTokenToValue(node->token);
        }
      }
      
      case ExprNodeType::OPERATOR: {
        if (node->children.size() != 2) {
          throw TableError("Invalid operator node");
        }
        
        Value left = evaluateExpression(node->children[0].get(), row);
        Value right = evaluateExpression(node->children[1].get(), row);
        
        switch (node->token.type) {
          case TokenType::PLUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
              return std::get<int>(left) + std::get<int>(right);
            }
            if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
              double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
              double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
              return l + r;
            }
            throw TableError("Invalid types for addition");
            
          case TokenType::MINUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
              return std::get<int>(left) - std::get<int>(right);
            }
            if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
              double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
              double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
              return l - r;
            }
            throw TableError("Invalid types for subtraction");
            
          case TokenType::ASTERISK:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
              return std::get<int>(left) * std::get<int>(right);
            }
            if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
              double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
              double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
              return l * r;
            }
            throw TableError("Invalid types for multiplication");
            
          default:
            throw TableError("Unknown operator");
        }
      }
      
      case ExprNodeType::PARENTHESIS: {
        if (node->children.size() != 1) {
          throw TableError("Invalid parenthesis node");
        }
        return evaluateExpression(node->children[0].get(), row);
      }
      
      default:
        throw TableError("Unknown expression node type");
    }
  }

  // Helper function to quote string
  static std::string quoteString(const std::string& str) {
    return "\"" + str + "\"";
  }

  // Helper function to read a quoted string
  static std::string readQuotedString(std::istream& in) {
    char c;
    std::string result;
    
    // Skip leading whitespace
    while (in.get(c) && std::isspace(c)) {}
    in.unget();
    
    if (in.peek() != '"') {
      throw TableError("Expected quoted string");
    }
    
    in.get(); // Skip opening quote
    while (in.get(c) && c != '"') {
      result += c;
    }
    
    if (c != '"') {
      throw TableError("Unterminated quoted string");
    }
    
    return result;
  }

  void serialize(std::ofstream &out) const {
    // Write table name
    out << "TABLE " << quoteString(name) << "\n";
    
    // Write columns
    out << "COLUMNS " << columns.size() << "\n";
    for (const auto &col : columns) {
      out << quoteString(col.name) << " " << TOKEN_STR.find(col.type)->second << "\n";
    }
    
    // Write rows
    out << "ROWS " << rows.size() << "\n";
    for (const auto &row : rows) {
      for (size_t i = 0; i < row.size(); ++i) {
        if (i > 0) out << " ";
        const auto &value = row[i];
        if (std::holds_alternative<int>(value)) {
          out << "INT " << std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
          out << "FLOAT " << std::get<double>(value);
        } else if (std::holds_alternative<std::string>(value)) {
          out << "TEXT " << quoteString(std::get<std::string>(value));
        }
      }
      out << "\n";
    }
  }

  static std::unique_ptr<Table> deserialize(std::ifstream &in) {
    std::string line, word;
    
    // Read table name
    std::getline(in, line);
    std::istringstream iss(line);
    iss >> word; // Skip "TABLE"
    std::string table_name = readQuotedString(iss);
    
    // Read columns
    std::getline(in, line);
    iss.clear();
    iss.str(line);
    iss >> word; // Skip "COLUMNS"
    size_t num_columns;
    iss >> num_columns;
    
    std::vector<ColumnDefinition> columns;
    columns.reserve(num_columns);
    
    for (size_t i = 0; i < num_columns; ++i) {
      std::getline(in, line);
      iss.clear();
      iss.str(line);
      
      ColumnDefinition col;
      col.name = readQuotedString(iss);
      std::string type_str;
      iss >> type_str;
      
      // Convert type string to TokenType
      bool found = false;
      for (const auto &[token_type, str] : TOKEN_STR) {
        if (str == type_str) {
          col.type = token_type;
          found = true;
          break;
        }
      }
      
      if (!found) {
        throw TableError("Invalid column type: " + type_str);
      }
      
      columns.push_back(col);
    }
    
    // Create table
    auto table = std::make_unique<Table>(table_name, columns);
    
    // Read rows
    std::getline(in, line);
    iss.clear();
    iss.str(line);
    iss >> word; // Skip "ROWS"
    size_t num_rows;
    iss >> num_rows;
    
    for (size_t i = 0; i < num_rows; ++i) {
      std::getline(in, line);
      iss.clear();
      iss.str(line);
      
      std::vector<Value> row;
      row.reserve(num_columns);
      
      for (size_t j = 0; j < num_columns; ++j) {
        std::string type;
        iss >> type;
        
        if (type == "INT") {
          int value;
          iss >> value;
          row.push_back(value);
        } else if (type == "FLOAT") {
          double value;
          iss >> value;
          row.push_back(value);
        } else if (type == "TEXT") {
          std::string value = readQuotedString(iss);
          row.push_back(value);
        } else {
          throw TableError("Invalid value type: " + type);
        }
      }
      
      table->rows.push_back(std::move(row));
    }
    
    return table;
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
      case TokenType::INEQUALS:
        return row_val != val;
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
      case TokenType::INEQUALS:
        return row_val != val;
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
      case TokenType::INEQUALS:
        return row_val != val;
      default:
        throw TableError("Invalid condition type");
      }
    } else {
      throw TableError("Value types do not match");
    }
  }
};