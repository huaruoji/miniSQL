#pragma once
#include "parser.hpp"
#include "sql_statement.hpp"
#include "utils.hpp"
#include <list>
#include <string>
#include <vector>

class Table {
public:
  Table(const std::string &name, const std::vector<ColumnDefinition> &columns);

  void insert(const std::vector<Value> &values);
  void select(const SelectStatement &stmt);
  void update(const UpdateStatement &stmt);
  void deleteRows(const DeleteStatement &stmt);

private:
  std::string name;
  bool compareValues(const std::string &column_name, TokenType condition_type,
                     const Value &value, const std::vector<Value> &row);
  std::vector<ColumnDefinition> columns;
  std::list<std::vector<Value>> rows;
  std::unordered_map<std::string, size_t> column_index;
};
;