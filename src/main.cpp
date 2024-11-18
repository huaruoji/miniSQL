#include "database.hpp"
#include "parser.hpp"
#include "sql_statement.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>

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

    // Parse statements and execute
    for (const auto &statement : statements) {
      std::unique_ptr<SQLStatement> parsed_statement =
          Parser(statement).parse();
    }
  } catch (const ArgumentError &e) {
    std::cerr << "Error: " << e.what() << "\n"
              << "Usage: "
              << "minidb"
              << " <input.sql> <output.txt>\n";
    return EXIT_FAILURE;
  } catch (const FileError &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  } catch (const DatabaseError &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  } catch (const std::exception &e) {
    std::cerr << "Unexpected Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}