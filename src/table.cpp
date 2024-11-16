#include "table.hpp"

Table::Table(const std::string &name,
             const std::vector<ColumnDefinition> &columns)
    : name(name), columns(columns) {
  // TODO: Initialize table with name and column definitions
}

void Table::insert(const std::vector<std::string> &values) {
  // TODO: Validate and insert values into table
  // - Check if number of values matches number of columns
  // - Convert values to appropriate types based on column definitions
  // - Add new row to table
}

std::vector<std::vector<std::string>>
Table::select(const SelectStatement &stmt) {
  // TODO: Implement select operation
  // - Filter rows based on where condition
  // - Project only requested columns
  // - Handle joins if specified
  // - Format results as strings
  return {};
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

void Table::saveToFile() const {
  // TODO: Implement table persistence
  // - Save table schema (column definitions)
  // - Save all rows in appropriate format
}

void Table::loadFromFile() {
  // TODO: Implement table loading
  // - Load and validate table schema
  // - Load all rows and convert to appropriate types
}