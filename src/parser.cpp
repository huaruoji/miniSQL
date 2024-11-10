#include "parser.hpp"
#include "database.hpp"
#include <stdexcept>

Parser::Parser(const std::string &input)
    : lexer_(input), current_token(nullptr), token_index(0) {
  // 初始化时获取所有token
  utils::Token token = lexer_.nextToken();
  while (token.type != utils::TokenType::END_OF_FILE) {
    tokens.push_back(token);
    token = lexer_.nextToken();
  }
  tokens.push_back(token); // 添加EOF标记

  if (!tokens.empty()) {
    current_token = &tokens[0];
  }
}

void Parser::parseStatement(Database &db) {
  if (!current_token) {
    throwError("No tokens to parse");
  }

  Query query;
  switch (current_token->type) {
  case utils::TokenType::KEYWORD:
    if (current_token->value == "CREATE") {
      advance();
      if (current_token->value == "DATABASE") {
        query = parseCreateDatabase();
      } else if (current_token->value == "TABLE") {
        query = parseCreateTable();
      } else {
        throwError("Expected DATABASE or TABLE after CREATE");
      }
    }
    // TODO: 处理其他类型的语句
    break;
  default:
    throwError("Unexpected token at start of statement");
  }

  // TODO: 执行查询
}

Query Parser::parseCreateDatabase() {
  Query query;
  query.type = QueryType::CREATE_DATABASE;

  advance(); // 跳过 DATABASE 关键字
  if (!current_token || current_token->type != utils::TokenType::IDENTIFIER) {
    throwError("Expected database name");
  }

  query.databaseName = current_token->value;
  advance();

  return query;
}

Query Parser::parseCreateTable() {
  // TODO: 实现创建表的解析
  return Query{};
}

Query Parser::parseUseDatabase() {
  // TODO: 实现使用数据库的解析
  return Query{};
}

Query Parser::parseDropTable() {
  // TODO: 实现删除表的解析
  return Query{};
}

Query Parser::parseInsert() {
  // TODO: 实现插入的解析
  return Query{};
}

Query Parser::parseSelect() {
  // TODO: 实现查询的解析
  return Query{};
}

Query Parser::parseUpdate() {
  // TODO: 实现更新的解析
  return Query{};
}

Query Parser::parseDelete() {
  // TODO: 实现删除的解析
  return Query{};
}

std::vector<utils::Condition> Parser::parseWhereClause() {
  // TODO: 实现WHERE子句的解析
  return {};
}

std::vector<utils::JoinClause> Parser::parseJoinClauses() {
  // TODO: 实现JOIN子句的解析
  return {};
}

utils::ColumnRef Parser::parseColumnRef() {
  // TODO: 实现列引用的解析
  return utils::ColumnRef();
}

void Parser::advance() {
  if (token_index < tokens.size() - 1) {
    token_index++;
    current_token = &tokens[token_index];
  } else {
    current_token = nullptr;
  }
}

void Parser::throwError(const std::string &message) {
  size_t line = current_token ? current_token->line : 0;
  size_t column = current_token ? current_token->column : 0;
  throw utils::SQLError(message, line, column);
}
