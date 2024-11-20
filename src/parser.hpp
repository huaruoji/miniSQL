#pragma once
#include "lexer.hpp"
#include "sql_statement.hpp"
#include "utils.hpp"
#include <memory>
#include <vector>

class Parser {
public:
  explicit Parser(const std::string &input);
  std::unique_ptr<SQLStatement> parse();

private:
  Lexer lexer;
  Token current_token;

  void advance();
  bool match(TokenType type);
  bool consume(TokenType type);
  bool exist(TokenType type);

  std::unique_ptr<WhereCondition> parseWhereCondition();
  std::unique_ptr<CreateDatabaseStatement> parseCreateDatabase();
  std::unique_ptr<UseDatabaseStatement> parseUseDatabase();
  std::unique_ptr<CreateTableStatement> parseCreateTable();
  std::unique_ptr<DropTableStatement> parseDropTable();
  std::unique_ptr<InsertStatement> parseInsert();
  std::unique_ptr<SelectStatement> parseSelect();
  std::unique_ptr<UpdateStatement> parseUpdate();
  std::unique_ptr<DeleteStatement> parseDelete();
  std::unique_ptr<InnerJoinStatement> parseInnerJoin();
};