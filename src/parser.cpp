#include "parser.hpp"

Parser::Parser(const std::string &input)
    : lexer(input), current_token(TokenType::EOF_TOKEN) {
  advance();
}

void Parser::advance() {
  // TODO: Get next token
}

bool Parser::match(TokenType type) {
  // TODO: Check if current token matches type
  return false;
}

void Parser::expect(TokenType type) {
  // TODO: Verify current token is of expected type
}

std::unique_ptr<SQLStatement> Parser::parse() {
  // TODO: Parse SQL statement
  return nullptr;
}

std::unique_ptr<SQLStatement> Parser::parseStatement() {
  // TODO: Parse different types of SQL statements
  return nullptr;
}

std::unique_ptr<CreateDatabaseStatement> Parser::parseCreateDatabase() {
  // TODO: Parse CREATE DATABASE statement
  return nullptr;
}

std::unique_ptr<UseDatabaseStatement> Parser::parseUseDatabase() {
  // TODO: Parse USE DATABASE statement
  return nullptr;
}

std::unique_ptr<CreateTableStatement> Parser::parseCreateTable() {
  // TODO: Parse CREATE TABLE statement
  return nullptr;
}

std::unique_ptr<DropTableStatement> Parser::parseDropTable() {
  // TODO: Parse DROP TABLE statement
  return nullptr;
}

std::unique_ptr<InsertStatement> Parser::parseInsert() {
  // TODO: Parse INSERT statement
  return nullptr;
}

std::unique_ptr<SelectStatement> Parser::parseSelect() {
  // TODO: Parse SELECT statement
  return nullptr;
}

std::unique_ptr<UpdateStatement> Parser::parseUpdate() {
  // TODO: Parse UPDATE statement
  return nullptr;
}

std::unique_ptr<DeleteStatement> Parser::parseDelete() {
  // TODO: Parse DELETE statement
  return nullptr;
}