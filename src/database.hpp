#pragma once
#include "statement.hpp"
#include "table.hpp"
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Database {
public:
  explicit Database(const std::string &name,
                    const std::string &data_dir = "data")
      : name(name), data_dir(data_dir) {}

  ~Database() {
    namespace fs = std::filesystem;
    if (!fs::exists(data_dir)) {
      fs::create_directory(data_dir);
    }
    fs::path db_path = fs::path(data_dir) / (name + ".db");
    serialize(db_path.string());
  }

  void executeStatement(SQLStatement *stmt) {
    switch (stmt->type) {
    case SQLStatementType::CREATE_TABLE: {
      auto create_stmt = static_cast<CreateTableStatement *>(stmt);
      if (tables.find(create_stmt->table_name) != tables.end()) {
        throw DatabaseError("Table already exists", create_stmt->line_number);
      }
      tables[create_stmt->table_name] = std::make_unique<Table>(
          create_stmt->table_name, create_stmt->columns);
      break;
    }
    case SQLStatementType::DROP_TABLE: {
      auto drop_stmt = static_cast<DropTableStatement *>(stmt);
      if (tables.find(drop_stmt->table_name) == tables.end()) {
        throw DatabaseError("Table does not exist", drop_stmt->line_number);
      }
      tables.erase(drop_stmt->table_name);
      break;
    }
    case SQLStatementType::INSERT: {
      auto insert_stmt = static_cast<InsertStatement *>(stmt);
      if (tables.find(insert_stmt->table_name) == tables.end()) {
        throw DatabaseError("Table does not exist", insert_stmt->line_number);
      }
      tables[insert_stmt->table_name]->insert(insert_stmt->values);
      break;
    }
    case SQLStatementType::SELECT: {
      auto select_stmt = static_cast<SelectStatement *>(stmt);
      if (tables.find(select_stmt->table_name) == tables.end()) {
        throw DatabaseError("Table does not exist", select_stmt->line_number);
      }
      tables[select_stmt->table_name]->select(*select_stmt);
      break;
    }
    case SQLStatementType::UPDATE: {
      auto update_stmt = static_cast<UpdateStatement *>(stmt);
      if (tables.find(update_stmt->table_name) == tables.end()) {
        throw DatabaseError("Table does not exist", update_stmt->line_number);
      }
      tables[update_stmt->table_name]->update(*update_stmt);
      break;
    }
    case SQLStatementType::DELETE: {
      auto delete_stmt = static_cast<DeleteStatement *>(stmt);
      if (tables.find(delete_stmt->table_name) == tables.end()) {
        throw DatabaseError("Table does not exist", delete_stmt->line_number);
      }
      tables[delete_stmt->table_name]->deleteRows(*delete_stmt);
      break;
    }
    case SQLStatementType::INNER_JOIN: {
      auto inner_join_stmt = static_cast<InnerJoinStatement *>(stmt);
      if (tables.find(inner_join_stmt->table_name_a) == tables.end() ||
          tables.find(inner_join_stmt->table_name_b) == tables.end()) {
        throw DatabaseError("Table does not exist",
                            inner_join_stmt->line_number);
      }
      tables[inner_join_stmt->table_name_a]->innerJoin(
          *inner_join_stmt, *tables[inner_join_stmt->table_name_b]);
      break;
    }
    default:
      throw DatabaseError("Unknown statement type", 0);
    }
  }

  std::string getName() const { return name; }

  // Serialize database to a file
  void serialize(const std::string &filepath) const {
    std::ofstream out(filepath);
    if (!out) {
      throw DatabaseError("Failed to open file for serialization", 0);
    }

    // Write database name
    out << "DATABASE " << name << "\n";

    // Write number of tables
    out << "TABLES " << tables.size() << "\n\n";

    // Write each table
    for (const auto &[table_name, table] : tables) {
      table->serialize(out);
      out << "\n"; // Add a blank line between tables
    }
  }

  // Deserialize database from a file
  static std::unique_ptr<Database> deserialize(const std::string &filepath) {
    std::ifstream in(filepath);
    if (!in) {
      throw DatabaseError("Failed to open file for deserialization", 0);
    }

    std::string line, word;

    // Read database name
    std::getline(in, line);
    std::istringstream iss(line);
    iss >> word; // Skip "DATABASE"
    std::string db_name;
    iss >> db_name;

    // Create database instance
    auto db = std::make_unique<Database>(db_name);

    // Read number of tables
    std::getline(in, line);
    iss.clear();
    iss.str(line);
    iss >> word; // Skip "TABLES"
    size_t num_tables;
    iss >> num_tables;

    // Skip the blank line
    std::getline(in, line);

    // Read each table
    for (size_t i = 0; i < num_tables; ++i) {
      auto table = Table::deserialize(in);
      db->tables[table->getName()] = std::move(table);

      // Skip the blank line between tables
      std::getline(in, line);
    }

    return db;
  }

private:
  std::string name;
  std::string data_dir;
  std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};