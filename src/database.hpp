#pragma once
#include "statement.hpp"
#include "table.hpp"
#include "utils.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Database {
public:
  explicit Database(const std::string &name) : name(name) {}

  void executeStatement(SQLStatement *stmt) {
    switch (stmt->type) {
    case SQLStatementType::CREATE_TABLE: {
      auto create_stmt = static_cast<CreateTableStatement *>(stmt);
      if (tables.find(create_stmt->table_name) != tables.end()) {
        throw DatabaseError("Table already exists");
      }
      tables[create_stmt->table_name] = std::make_unique<Table>(
          create_stmt->table_name, create_stmt->columns);
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
    case SQLStatementType::UPDATE: {
      auto update_stmt = static_cast<UpdateStatement *>(stmt);
      if (tables.find(update_stmt->table_name) == tables.end()) {
        throw DatabaseError("Table does not exist");
      }
      tables[update_stmt->table_name]->update(*update_stmt);
      break;
    }
    case SQLStatementType::DELETE: {
      auto delete_stmt = static_cast<DeleteStatement *>(stmt);
      if (tables.find(delete_stmt->table_name) == tables.end()) {
        throw DatabaseError("Table does not exist");
      }
      tables[delete_stmt->table_name]->deleteRows(*delete_stmt);
      break;
    }
    case SQLStatementType::INNER_JOIN: {
      auto inner_join_stmt = static_cast<InnerJoinStatement *>(stmt);
      if (tables.find(inner_join_stmt->table_name_a) == tables.end() ||
          tables.find(inner_join_stmt->table_name_b) == tables.end()) {
        throw DatabaseError("Table does not exist");
      }
      tables[inner_join_stmt->table_name_a]->innerJoin(
          *inner_join_stmt, *tables[inner_join_stmt->table_name_b]);
      break;
    }
    default:
      throw DatabaseError("Unknown statement type");
    }
  }

private:
  std::string name;
  std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};