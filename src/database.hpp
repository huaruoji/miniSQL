#pragma once
#include "table.hpp"
#include <unordered_map>

class Database {
public:
  void createTable(const std::string &name, const std::vector<Column> &columns);
  void dropTable(const std::string &name);
  void insert(const std::string &tableName, const std::vector<Value> &values);
  std::vector<std::vector<Value>>
  select(const std::string &tableName, const std::vector<std::string> &columns);
  void createIndex(const std::string &tableName, const std::string &columnName);

private:
  std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};