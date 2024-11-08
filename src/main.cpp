#include "database.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <string>


void executeFromFile(const std::string &filename, Database &db) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Cannot open file: " << filename << std::endl;
    return;
  }

  Parser parser;
  std::string currentQuery;
  std::string line;

  while (std::getline(file, line)) {
    // 跳过空行和注释
    if (line.empty() || line[0] == '#' || line[0] == '-')
      continue;

    // 移除前后空白
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);

    currentQuery += line + " ";

    if (line.back() == ';') {
      // 移除末尾分号
      currentQuery = currentQuery.substr(0, currentQuery.length() - 1);
      try {
        Query query = parser.parse(currentQuery);

        switch (query.type) {
        case QueryType::CREATE_TABLE:
          db.createTable(query.tableName, query.columns);
          std::cout << "Table " << query.tableName << " created." << std::endl;
          break;

        case QueryType::DROP_TABLE:
          db.dropTable(query.tableName);
          std::cout << "Table " << query.tableName << " dropped." << std::endl;
          break;

        case QueryType::INSERT:
          db.insert(query.tableName, query.values);
          std::cout << "1 row inserted into " << query.tableName << std::endl;
          break;

        case QueryType::SELECT: {
          auto results = db.select(query.tableName, query.selectColumns);
          for (const auto &row : results) {
            for (const auto &value : row) {
              std::visit([](const auto &v) { std::cout << v << "\t"; }, value);
            }
            std::cout << std::endl;
          }
        } break;

        case QueryType::CREATE_INDEX:
          db.createIndex(query.tableName, query.indexColumn);
          std::cout << "Index created on " << query.tableName << "("
                    << query.indexColumn << ")" << std::endl;
          break;
        }
      } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
      }

      currentQuery.clear();
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <sql_file>" << std::endl;
    return 1;
  }

  Database db;
  executeFromFile(argv[1], db);
  return 0;
}