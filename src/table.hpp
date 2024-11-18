#pragma once
#include "parser.hpp"
#include "sql_statement.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

class Row {
  std::vector<Value> values;
};

class Table {
public:
  Table(const std::string &name, const std::vector<ColumnDefinition> &columns);

  void insert(const std::vector<std::string> &values);
  std::vector<std::vector<std::string>> select(const SelectStatement &stmt);
  void update(const UpdateStatement &stmt);
  void deleteRows(const DeleteStatement &stmt);

  void saveToFile() const;
  void loadFromFile();

private:
  std::string name;
  std::vector<ColumnDefinition> columns;

  // 暂时使用 vector 存储数据，后面可能升级成 multiset，并且建立索引来支持 log
  // 复杂度的查询
  std::vector<Row> rows;
};