#include "database.hpp"
#include "parser.hpp"
#include "statement.hpp"
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

OutputWriter file_writer;
namespace fs = std::filesystem;

void loadDatabases(
    std::unordered_map<std::string, std::unique_ptr<Database>> &databases,
    const std::string &data_dir) {
  if (!fs::exists(data_dir)) {
    fs::create_directory(data_dir);
    return;
  }

  for (const auto &entry : fs::directory_iterator(data_dir)) {
    if (entry.path().extension() == ".db") {
      auto db = Database::deserialize(entry.path().string());
      databases[db->getName()] = std::move(db);
    }
  }
}

int main(int argc, char *argv[]) {
  std::unordered_map<std::string, std::unique_ptr<Database>> databases;
  Database *current_database = nullptr;
  const std::string data_dir = "data";

  try {
    if (argc != 3) {
      throw ArgumentError("Argument number error");
    } else if (std::string(argv[1]).find(".sql") == std::string::npos) {
      throw ArgumentError("Input file must be a SQL file");
    }

    // Load existing databases
    loadDatabases(databases, data_dir);

    // Read input SQL file and split into statements
    std::ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
      throw FileError("Failed to open input file");
    }
    const std::vector<Statement> &&statements = splitStatements(input_file);

    // Open output file for writing
    file_writer.open(argv[2]);

    // Parse statements and execute
    for (const auto &statement : statements) {
      std::unique_ptr<SQLStatement> parsed_statement =
          Parser(statement.content, statement.start_line).parse();
      if (parsed_statement->type == SQLStatementType::CREATE_DATABASE) {
        if (databases.find(parsed_statement->getDatabaseName()) !=
            databases.end()) {
          throw DatabaseError("Database already exists",
                              parsed_statement->line_number);
        }
        databases[parsed_statement->getDatabaseName()] =
            std::make_unique<Database>(parsed_statement->getDatabaseName(),
                                       data_dir);
      } else if (parsed_statement->type == SQLStatementType::USE_DATABASE) {
        if (databases.find(parsed_statement->getDatabaseName()) ==
            databases.end()) {
          throw DatabaseError("Database does not exist",
                              parsed_statement->line_number);
        }
        current_database = databases[parsed_statement->getDatabaseName()].get();
      } else {
        if (!current_database) {
          throw DatabaseError("No database selected",
                              parsed_statement->line_number);
        }
        current_database->executeStatement(parsed_statement.get());
      }
#ifdef DEBUG
      std::cerr << "Successfully parsed statement\n";
#endif
    }
  } catch (const ArgumentError &e) {
    std::cerr << "ArgumentError: " << e.what() << "\n"
              << "Usage: minidb <input_file.sql> <output_file.csv>\n";
    return EXIT_FAILURE;
  } catch (const FileError &e) {
    std::cerr << "File Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  } catch (const SQLError &e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
  } catch (const std::exception &e) {
    std::cerr << "Unexpected Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}