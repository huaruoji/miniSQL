#include "database.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " input.sql output.txt\n";
    return 1;
  }

  // TODO: Implement main program logic
  // 1. Read input SQL file
  // 2. Parse SQL statements
  // 3. Execute statements
  // 4. Write results to output file

  return 0;
}