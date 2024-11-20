#pragma once
#include "sql_statement.hpp"
#include "table.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Database {
public:
  explicit Database(const std::string &name);
  void executeStatement(SQLStatement *stmt);

private:
  std::string name;
  std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};