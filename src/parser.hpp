#pragma once
#include "lexer.hpp"
#include <memory>
#include <vector>


struct SQLStatement {
  virtual ~SQLStatement() = default;
};

struct CreateDatabaseStatement : SQLStatement {
  std::string database_name;
};

struct UseDatabaseStatement : SQLStatement {
  std::string database_name;
};

struct ColumnDefinition {
  std::string name;
  TokenType type;
};

struct CreateTableStatement : SQLStatement {
  std::string table_name;
  std::vector<ColumnDefinition> columns;
};

struct DropTableStatement : SQLStatement {
  std::string table_name;
};

struct InsertStatement : SQLStatement {
  std::string table_name;
  std::vector<std::string> values;
};

struct SelectStatement : SQLStatement {
  std::vector<std::string> columns;
  std::string table_name;
  std::string join_table;
  std::string join_condition;
  std::string where_condition;
};

struct UpdateStatement : SQLStatement {
  std::string table_name;
  std::vector<std::pair<std::string, std::string>> set_columns;
  std::string where_condition;
};

struct DeleteStatement : SQLStatement {
  std::string table_name;
  std::string where_condition;
};

class Parser {
public:
  explicit Parser(const std::string &input);
  std::unique_ptr<SQLStatement> parse();

private:
  Lexer lexer;
  Token current_token;

  void advance();
  bool match(TokenType type);
  void expect(TokenType type);

  std::unique_ptr<SQLStatement> parseStatement();
  std::unique_ptr<CreateDatabaseStatement> parseCreateDatabase();
  std::unique_ptr<UseDatabaseStatement> parseUseDatabase();
  std::unique_ptr<CreateTableStatement> parseCreateTable();
  std::unique_ptr<DropTableStatement> parseDropTable();
  std::unique_ptr<InsertStatement> parseInsert();
  std::unique_ptr<SelectStatement> parseSelect();
  std::unique_ptr<UpdateStatement> parseUpdate();
  std::unique_ptr<DeleteStatement> parseDelete();
};