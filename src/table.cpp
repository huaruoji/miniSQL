#include "table.hpp"
#include <algorithm>

Table::Table(const std::string &name,
             const std::vector<ColumnDefinition> &columns)
    : name(name), columns(columns) {
  // Build column index
  for (size_t i = 0; i < columns.size(); i++) {
    column_index[columns[i].name] = i;
  }
}

void Table::insert(const std::vector<Value> &row) {
  // Validate number of values matches number of columns
  std::cerr << row.size() << ' ' << columns.size() << std::endl;
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
      std::cerr << " Type: " << TOKEN_STR.find(columns[i].type)->second << ' ';
      throw TableError("Value does not match column type");
    }
  }

  // Store the row values in the table data
  rows.push_back(row);
}

bool Table::compareValues(const std::string &column_name,
                          TokenType condition_type, const Value &value,
                          const std::vector<Value> &row) {
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

void Table::select(const SelectStatement &stmt) {
  // Find rows matching where condition
  std::vector<std::vector<Value>> results;
  for (const auto &row : rows) {
    bool matches = true;
    if (stmt.where_condition) {
      matches = compareValues(stmt.where_condition->column_name_a,
                              stmt.where_condition->condition_type_a,
                              stmt.where_condition->value_a, row);
      if (stmt.where_condition->logic_operator == TokenType::AND) {
        matches =
            matches && compareValues(stmt.where_condition->column_name_b,
                                     stmt.where_condition->condition_type_b,
                                     stmt.where_condition->value_b, row);
      } else if (stmt.where_condition->logic_operator == TokenType::OR) {
        matches =
            matches || compareValues(stmt.where_condition->column_name_b,
                                     stmt.where_condition->condition_type_b,
                                     stmt.where_condition->value_b, row);
      }
    }
    if (matches) {
      if (stmt.columns.size() == 1 && stmt.columns[0] == "*") {
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
  if (!results.empty())
    file_writer.write(results);
}

void Table::update(const UpdateStatement &stmt) {
  // TODO: Implement update operation
  // - Find rows matching where condition
  // - Update specified columns with new values
  // - Validate new values match column types
}

void Table::deleteRows(const DeleteStatement &stmt) {
  // TODO: Implement delete operation
  // - Find rows matching where condition
  // - Remove matching rows from table
}