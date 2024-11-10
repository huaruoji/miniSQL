#pragma once
#include "lexer.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

enum class QueryType {
  CREATE_DATABASE,
  USE_DATABASE,
  CREATE_TABLE,
  DROP_TABLE,
  INSERT,
  SELECT,
  UPDATE,
  DELETE
};

struct Query {
  QueryType type;
  std::string databaseName;
  std::string tableName;
  std::vector<utils::Column> columns;
  std::vector<utils::ColumnRef> selectColumns;
  std::vector<utils::Value> values;
  std::vector<utils::Condition> whereConditions;
  std::vector<utils::JoinClause> joins;
  std::vector<std::pair<std::string, utils::Value>> updateAssignments;
};

class Parser {
public:
  Query parse(const std::vector<Token> &tokens);

private:
  const Token *current_token;
  std::vector<Token> tokens;
  size_t token_index;

  void advance();
  void throwError(const std::string &message);

  Query parseCreateDatabase();
  Query parseUseDatabase();
  Query parseCreateTable();
  Query parseDropTable();
  Query parseInsert();
  Query parseSelect();
  Query parseUpdate();
  Query parseDelete();
  std::vector<utils::Condition> parseWhereClause();
  std::vector<utils::JoinClause> parseJoinClauses();
  utils::ColumnRef parseColumnRef();
};