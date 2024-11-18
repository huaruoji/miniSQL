#pragma once
#include "sql_statement.hpp"
#include "table.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Database {
public:
  explicit Database(const std::string &name);

  void createTable(const std::string &name,
                   const std::vector<ColumnDefinition> &columns);
  void dropTable(const std::string &name);
  void insertIntoTable(const std::string &name,
                       const std::vector<std::string> &values);
  std::vector<std::vector<std::string>>
  selectFromTable(const SelectStatement &stmt);
  void updateTable(const UpdateStatement &stmt);
  void deleteFromTable(const DeleteStatement &stmt);

  void saveToFile() const;
  void loadFromFile();

private:
  std::string name;
  std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};