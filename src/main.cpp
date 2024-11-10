#include "database.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0]
              << " <input_file_name>.sql <output_file_name>.csv" << std::endl;
    return 1;
  }

  // 读取SQL文件内容
  std::ifstream inFile(argv[1]);
  if (!inFile) {
    std::cerr << "Cannot open input file: " << argv[1] << std::endl;
    return 1;
  }
  std::string content((std::istreambuf_iterator<char>(inFile)), {});

  // 准备输出CSV文件
  std::ofstream outFile(argv[2], std::ios::out | std::ios::trunc);
  if (!outFile) {
    std::cerr << "Cannot open output file: " << argv[2] << std::endl;
    return 1;
  }

  // 初始化数据库、解析器和词法分析器
  Database db;
  Parser parser;
  Lexer lexer(content);
  std::vector<Token> currentTokens;

  // Process tokens until end of file
  while (lexer.hasMoreTokens()) {
    Token token = lexer.nextToken();

    if (token.type == TokenType::END) {
      break;
    }

    // Add token to current collection
    currentTokens.push_back(token);

    // When we hit a semicolon, parse and execute the query
    if (token.type == TokenType::SEMICOLON) {
      try {
        Query query = parser.parse(currentTokens);

        switch (query.type) {
        case QueryType::CREATE_DATABASE:
          // TODO: 实现创建数据库的功能
          std::cout << "Database " << query.databaseName << " created."
                    << std::endl;
          break;

        case QueryType::USE_DATABASE:
          db.useDatabase(query.databaseName);
          std::cout << "Database changed to " << query.databaseName
                    << std::endl;
          break;

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
          std::vector<std::string> columnNames;
          if (query.selectColumns.empty()) {
            // 如果是 SELECT *，获取所有列名
            const Table *table = db.getTable(query.tableName);
            for (const auto &col : table->getColumns()) {
              columnNames.push_back(col.name);
            }
          } else {
            // 处理指定的列名
            for (const auto &colRef : query.selectColumns) {
              if (!colRef.table.empty()) {
                columnNames.push_back(colRef.table + "." + colRef.column);
              } else {
                columnNames.push_back(colRef.column);
              }
            }
          }

          auto results = db.select(query.tableName, columnNames,
                                   query.whereConditions, query.joins);

          // Write column headers to CSV
          for (size_t i = 0; i < columnNames.size(); ++i) {
            outFile << columnNames[i];
            if (i < columnNames.size() - 1) {
              outFile << ",";
            }
          }
          outFile << "\n";

          // Write data rows to CSV
          for (const auto &row : results) {
            for (size_t i = 0; i < row.size(); ++i) {
              std::visit([&outFile](const auto &v) { outFile << v; }, row[i]);
              if (i < row.size() - 1) {
                outFile << ",";
              }
            }
            outFile << "\n";
          }
        } break;

        case QueryType::UPDATE: {
          // TODO: 实现 UPDATE 操作
          std::cout << "Row(s) updated in " << query.tableName << std::endl;
        } break;

        case QueryType::DELETE: {
          // TODO: 实现 DELETE 操作
          std::cout << "Row(s) deleted from " << query.tableName << std::endl;
        } break;
        }
      } catch (const utils::SQLError &e) {
        std::cerr << "Error at line " << e.getLine() << ", column "
                  << e.getColumn() << ": " << e.what() << std::endl;

        // Build the query string for error reporting
        std::string queryStr;
        for (const auto &t : currentTokens) {
          queryStr += t.value + " ";
        }
        std::cerr << "In query: " << queryStr << std::endl;

        if (e.getColumn() > 0) {
          std::cerr << std::string(e.getColumn() - 1, ' ') << "^" << std::endl;
        }
      } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
      }

      // Clear tokens after execution
      currentTokens.clear();
    }
  }

  outFile.close();
  return 0;
}