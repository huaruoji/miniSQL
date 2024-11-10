#pragma once
#include "table.hpp"
#include "utils.hpp"
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

class Database {
public:
  explicit Database(std::string name);

  // Database operations
  void createDatabase(const std::string &name);
  void useDatabase(const std::string &name);
  void createTable(const std::string &name,
                   const std::vector<utils::Column> &columns);
  void dropTable(const std::string &name);

  // Data manipulation
  void insert(const std::string &tableName,
              const std::vector<utils::Value> &values);
  void
  update(const std::string &tableName,
         const std::vector<std::pair<std::string, utils::Value>> &assignments,
         const std::vector<utils::Condition> &conditions);
  void deleteFrom(const std::string &tableName,
                  const std::vector<utils::Condition> &conditions);

  // Queries
  std::vector<std::vector<utils::Value>>
  select(const std::string &tableName, const std::vector<std::string> &columns,
         const std::vector<utils::Condition> &conditions,
         const std::vector<utils::JoinClause> &joins);

  // Table access
  Table *getTable(const std::string &name);
  const Table *getTable(const std::string &name) const;
  bool hasTable(const std::string &name) const;

private:
  std::string name_;
  std::string currentDatabase;
  std::map<std::string, std::unordered_map<std::string, std::unique_ptr<Table>>>
      databases;

  void checkDatabaseExists(const std::string &name) const;
  void checkDatabaseSelected() const;
};