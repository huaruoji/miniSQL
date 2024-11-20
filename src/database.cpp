#include "database.hpp"
#include "sql_statement.hpp"
#include "utils.hpp"
#include <stdexcept>

Database::Database(const std::string &name) : name(name) {}

void Database::executeStatement(SQLStatement *stmt) {
  switch (stmt->type) {
  case SQLStatementType::CREATE_TABLE: {
    auto create_stmt = static_cast<CreateTableStatement *>(stmt);
    if (tables.find(create_stmt->table_name) != tables.end()) {
      throw DatabaseError("Table already exists");
    }
    tables[create_stmt->table_name] =
        std::make_unique<Table>(create_stmt->table_name, create_stmt->columns);
    break;
  }
  case SQLStatementType::DROP_TABLE: {
    auto drop_stmt = static_cast<DropTableStatement *>(stmt);
    if (tables.find(drop_stmt->table_name) == tables.end()) {
      throw DatabaseError("Table does not exist");
    }
    tables.erase(drop_stmt->table_name);
    break;
  }
  case SQLStatementType::INSERT: {
    auto insert_stmt = static_cast<InsertStatement *>(stmt);
    std::cerr << insert_stmt->table_name << insert_stmt->values.size()
              << std::endl;
    if (tables.find(insert_stmt->table_name) == tables.end()) {
      throw DatabaseError("Table does not exist");
    }
    tables[insert_stmt->table_name]->insert(insert_stmt->values);
    break;
  }
  case SQLStatementType::SELECT: {
    auto select_stmt = static_cast<SelectStatement *>(stmt);
    if (tables.find(select_stmt->table_name) == tables.end()) {
      throw DatabaseError("Table does not exist");
    }
    tables[select_stmt->table_name]->select(*select_stmt);
    break;
  }
  default:
    throw DatabaseError("Unknown statement type");
  }
}