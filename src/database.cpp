#include "database.hpp"
#include <stdexcept>

void Database::createDatabase(const std::string &name) {
  // TODO: 检查数据库是否已存在
  databases[name] = std::unordered_map<std::string, std::unique_ptr<Table>>();
}

void Database::useDatabase(const std::string &name) {
  checkDatabaseExists(name);
  currentDatabase = name;
}

void Database::createTable(const std::string &name,
                           const std::vector<utils::Column> &columns) {
  checkDatabaseSelected();
  auto &db = databases[currentDatabase];
  if (db.find(name) != db.end()) {
    throw std::runtime_error("Table already exists: " + name);
  }
  db[name] = std::make_unique<Table>(name, columns);
}

void Database::dropTable(const std::string &name) {
  checkDatabaseSelected();
  auto &db = databases[currentDatabase];
  if (db.find(name) == db.end()) {
    throw std::runtime_error("Table does not exist: " + name);
  }
  db.erase(name);
}

void Database::insert(const std::string &tableName,
                      const std::vector<utils::Value> &values) {
  Table *table = getTable(tableName);
  table->insert(values);
}

void Database::update(
    const std::string &tableName,
    const std::vector<std::pair<std::string, utils::Value>> &assignments,
    const std::vector<utils::Condition> &conditions) {
  Table *table = getTable(tableName);
  table->update(assignments, conditions);
}

void Database::deleteFrom(const std::string &tableName,
                          const std::vector<utils::Condition> &conditions) {
  Table *table = getTable(tableName);
  table->deleteRows(conditions);
}

std::vector<std::vector<utils::Value>>
Database::select(const std::string &tableName,
                 const std::vector<std::string> &columns,
                 const std::vector<utils::Condition> &conditions,
                 const std::vector<utils::JoinClause> &joins) {
  Table *table = getTable(tableName);
  return table->select(columns, conditions, joins);
}

void Database::checkDatabaseExists(const std::string &name) const {
  if (databases.find(name) == databases.end()) {
    throw std::runtime_error("Database does not exist: " + name);
  }
}

void Database::checkDatabaseSelected() const {
  if (currentDatabase.empty()) {
    throw std::runtime_error("No database selected");
  }
}

Table *Database::getTable(const std::string &name) {
  checkDatabaseSelected();
  auto &db = databases[currentDatabase];
  auto it = db.find(name);
  if (it == db.end()) {
    throw std::runtime_error("Table does not exist: " + name);
  }
  return it->second.get();
}

const Table *Database::getTable(const std::string &name) const {
  checkDatabaseSelected();
  const auto &db = databases.at(currentDatabase);
  auto it = db.find(name);
  if (it == db.end()) {
    throw std::runtime_error("Table does not exist: " + name);
  }
  return it->second.get();
}