#include "database.hpp"
#include "sql_statement.hpp"
#include "utils.hpp"
#include <stdexcept>

Database::Database(const std::string &name) : name(name) {}

void Database::createTable(const std::string &name,
                           const std::vector<ColumnDefinition> &columns) {
  if (tables.find(name) != tables.end()) {
    throw DatabaseError("Table already exists");
  }
  tables[name] = std::make_unique<Table>(name, columns);
}

void Database::dropTable(const std::string &name) {
  if (tables.find(name) == tables.end()) {
    throw DatabaseError("Table does not exist");
  }
  tables.erase(name);
}

void Database::insertIntoTable(const std::string &name,
                               const std::vector<std::string> &values) {
  // TODO: Implement record insertion
}

std::vector<std::vector<std::string>>
Database::selectFromTable(const SelectStatement &stmt) {
  // TODO: Implement table querying
  return {};
}

void Database::updateTable(const UpdateStatement &stmt) {
  // TODO: Implement record updating
}

void Database::deleteFromTable(const DeleteStatement &stmt) {
  // TODO: Implement record deletion
}

void Database::saveToFile() const {
  // TODO: Implement database persistence
}

void Database::loadFromFile() {
  // TODO: Implement database loading
}