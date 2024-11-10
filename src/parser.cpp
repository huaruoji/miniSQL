#include "parser.hpp"
#include "utils.hpp"
#include <stdexcept>

Query Parser::parse(const std::vector<Token> &tokens) {
  if (tokens.empty()) {
    throwError("Empty query");
  }

  this->tokens = tokens;
  this->token_index = 0;
  advance();

  switch (current_token->type) {
  case TokenType::CREATE: {
    advance();
    if (current_token->type == TokenType::TABLE) {
      return parseCreateTable();
    } else if (current_token->type == TokenType::DATABASE) {
      return parseCreateDatabase();
    } else {
      throwError("Expected DATABASE or TABLE after CREATE");
    }
  }
  case TokenType::USE: {
    advance();
    if (current_token->type == TokenType::DATABASE) {
      return parseUseDatabase();
    } else {
      throwError("Expected DATABASE after USE");
    }
  }
  case TokenType::DROP: {
    advance();
    if (current_token->type == TokenType::TABLE) {
      return parseDropTable();
    } else {
      throwError("Expected TABLE after DROP");
    }
  }
  case TokenType::INSERT:
    return parseInsert();
  case TokenType::SELECT:
    return parseSelect();
  case TokenType::UPDATE:
    return parseUpdate();
  case TokenType::DELETE:
    return parseDelete();
  default:
    throwError("Unexpected token at start of query: " + current_token->value);
  }
  return Query{};
}

Query Parser::parseCreateDatabase() {
  // TODO: 实现 CREATE DATABASE 语句的解析
  // 1. 解析数据库名
  // 2. 验证语法正确性
  return Query{};
}

Query Parser::parseUseDatabase() {
  // TODO: 实现 USE DATABASE 语句的解析
  // 1. 解析数据库名
  // 2. 验证语法正确性
  return Query{};
}

Query Parser::parseCreateTable() {
  // TODO: 实现 CREATE TABLE 语句的解析
  // 1. 解析表名
  // 2. 解析列定义列表
  // 3. 验证语法正确性
  return Query{};
}

Query Parser::parseDropTable() {
  // TODO: 实现 DROP TABLE 语句的解析
  // 1. 解析表名
  // 2. 验证语法正确性
  return Query{};
}

Query Parser::parseInsert() {
  // TODO: 实现 INSERT 语句的解析
  // 1. 解析表名
  // 2. 解析 VALUES 关键字
  // 3. 解析值列表
  // 4. 验证语法正确性
  return Query{};
}

Query Parser::parseSelect() {
  // TODO: 实现 SELECT 语句的解析
  // 1. 解析选择的列（包括 *）
  // 2. 解析 FROM 子句
  // 3. 解析可选的 JOIN 子句
  // 4. 解析可选的 WHERE 子句
  // 5. 验证语法正确性
  return Query{};
}

Query Parser::parseUpdate() {
  // TODO: 实现 UPDATE 语句的解析
  // 1. 解析表名
  // 2. 解析 SET 子句
  // 3. 解析可选的 WHERE 子句
  // 4. 验证语法正确性
  return Query{};
}

Query Parser::parseDelete() {
  // TODO: 实现 DELETE 语句的解析
  // 1. 解析表名
  // 2. 解析可选的 WHERE 子句
  // 3. 验证语法正确性
  return Query{};
}

std::vector<utils::Condition> Parser::parseWhereClause() {
  // TODO: 实现 WHERE 子句的解析
  // 1. 解析条件列表（支持 AND）
  // 2. 支持表名限定的列引用
  return {};
}

std::vector<utils::JoinClause> Parser::parseJoinClauses() {
  // TODO: 实现 JOIN 子句的解析
  // 1. 解析 INNER JOIN
  // 2. 解析 ON 条件
  return {};
}

utils::ColumnRef Parser::parseColumnRef() {
  // TODO: 实现列引用的解析
  // 1. 解析可能带表名限定的列名
  return {};
}
