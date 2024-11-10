#include "database.hpp"
#include "parser.hpp"
#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::string current_line;
  std::stringstream query_buffer;
  Database *current_db = nullptr;

  try {
    while (std::getline(std::cin, current_line)) {
      query_buffer << current_line << '\n';

      // Check if we have a complete statement (ends with semicolon)
      if (current_line.find(';') != std::string::npos) {
        Parser parser(query_buffer.str());
        parser.parseStatement(*current_db);
        query_buffer.str(""); // Clear buffer
        query_buffer.clear();
      }
    }
  } catch (const utils::SQLError &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}