#include "database.hpp"
#include "parser.hpp"
#include "statement.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>

FileWriter file_writer;

int main(int argc, char *argv[]) {
  std::unordered_map<std::string, std::unique_ptr<Database>> databases;
  Database *current_database = nullptr;
  try {
    if (argc != 3) {
      throw ArgumentError("Argument number error");
    } else if (std::string(argv[1]).find(".sql") == std::string::npos) {
      throw ArgumentError("Input file must be a SQL file");
    }

    // Read input SQL file and split into statements
    std::ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
      throw FileError("Failed to open input file");
    }
    const std::vector<std::string> &&statements = splitStatements(input_file);

    // Open output file for writing
    file_writer.open(argv[2]);

    // Parse statements and execute
    for (const auto &statement : statements) {
      std::unique_ptr<SQLStatement> parsed_statement =
          Parser(statement).parse();
      std::cerr << SQL_STATEMENT_TYPE_STR.find(parsed_statement->type)->second
                << "\n";
      if (parsed_statement->type == SQLStatementType::CREATE_DATABASE) {
        if (databases.find(parsed_statement->getDatabaseName()) !=
            databases.end()) {
          throw DatabaseError("Database already exists");
        }
        databases[parsed_statement->getDatabaseName()] =
            std::make_unique<Database>(parsed_statement->getDatabaseName());
      } else if (parsed_statement->type == SQLStatementType::USE_DATABASE) {
        if (databases.find(parsed_statement->getDatabaseName()) ==
            databases.end()) {
          throw DatabaseError("Database does not exist");
        }
        current_database = databases[parsed_statement->getDatabaseName()].get();
      } else {
        current_database->executeStatement(parsed_statement.get());
      }
      std::cerr << "Successfully parsed statement\n";
    }
  } catch (const ArgumentError &e) {
    std::cerr << "Error: " << e.what() << "\n"
              << "Usage: minidb <input_file.sql> <output_file.csv>\n";
    return EXIT_FAILURE;
  } catch (const FileError &e) {
    std::cerr << "File Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  } catch (const DatabaseError &e) {
    std::cerr << "Database Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  } catch (const TableError &e) {
    std::cerr << "Table Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  } catch (const ParseError &e) {
    std::cerr << "Parse Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  } catch (const std::exception &e) {
    std::cerr << "Unexpected Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}