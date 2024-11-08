#pragma once
#include "table.hpp"
#include <string>
#include <vector>


enum class QueryType { CREATE_TABLE, DROP_TABLE, INSERT, SELECT, CREATE_INDEX };

struct Query {
  QueryType type;
  std::string tableName;
  std::vector<Column> columns;            // 用于CREATE TABLE
  std::vector<std::string> selectColumns; // 用于SELECT
  std::vector<Value> values;              // 用于INSERT
  std::string indexColumn;                // 用于CREATE INDEX
};

class Parser {
public:
  Query parse(const std::string &sql);

private:
  Query parseCreateTable(const std::string &sql);
  Query parseDropTable(const std::string &sql);
  Query parseInsert(const std::string &sql);
  Query parseSelect(const std::string &sql);
  Query parseCreateIndex(const std::string &sql);
};