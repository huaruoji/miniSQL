#include "database.hpp"
#include <stdexcept>

Database::Database(const std::string &name) : name(name) {}

void Database::createTable(const std::string &name,
                           const std::vector<ColumnDefinition> &columns) {
  // TODO: Implement table creation
}

void Database::dropTable(const std::string &name) {
  // TODO: Implement table deletion
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

DatabaseManager &DatabaseManager::getInstance() {
  static DatabaseManager instance;
  return instance;
}

void DatabaseManager::createDatabase(const std::string &name) {
  // TODO: Implement database creation
}

void DatabaseManager::useDatabase(const std::string &name) {
  // TODO: Implement database selection
}

Database &DatabaseManager::getCurrentDatabase() {
  if (!current_database) {
    throw std::runtime_error("No database selected");
  }
  return *current_database;
}