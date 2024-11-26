#pragma once

#include "utils.hpp"
#include <memory>
#include <string>
#include <vector>

struct SQLStatement {
  SQLStatementType type;
  int line_number;

  SQLStatement() : line_number(0) {}
  virtual ~SQLStatement() = default;
  virtual std::string getDatabaseName() const { return ""; }
};

// SQL statement types

struct CreateDatabaseStatement : SQLStatement {
  CreateDatabaseStatement() { type = SQLStatementType::CREATE_DATABASE; }
  std::string database_name;
  std::string getDatabaseName() const override { return database_name; }
};

struct UseDatabaseStatement : SQLStatement {
  UseDatabaseStatement() { type = SQLStatementType::USE_DATABASE; }
  std::string database_name;
  std::string getDatabaseName() const override { return database_name; }
};

struct CreateTableStatement : SQLStatement {
  CreateTableStatement() { type = SQLStatementType::CREATE_TABLE; }
  std::string table_name;
  std::vector<ColumnDefinition> columns;
};

struct DropTableStatement : SQLStatement {
  DropTableStatement() { type = SQLStatementType::DROP_TABLE; }
  std::string table_name;
};

struct InsertStatement : SQLStatement {
  InsertStatement() { type = SQLStatementType::INSERT; }
  std::string table_name;
  std::vector<Value> values;
};

struct SelectStatement : SQLStatement {
  SelectStatement() { type = SQLStatementType::SELECT; }
  std::vector<std::string> columns;
  std::string table_name;
  std::unique_ptr<WhereCondition> where_condition;
};

struct InnerJoinStatement : SQLStatement {
  InnerJoinStatement() { type = SQLStatementType::INNER_JOIN; }
  std::string table_name_a;
  std::string table_name_b;
  std::string column_name_a;
  std::string column_name_b;
  std::string condition_column_name_a;
  std::string condition_column_name_b;
  TokenType condition_type;
};

struct UpdateStatement : SQLStatement {
  UpdateStatement() { type = SQLStatementType::UPDATE; }
  std::string table_name;
  std::vector<SetCondition> set_conditions;
  std::unique_ptr<WhereCondition> where_condition;
};

struct DeleteStatement : SQLStatement {
  DeleteStatement() { type = SQLStatementType::DELETE; }
  std::string table_name;
  std::unique_ptr<WhereCondition> where_condition;
};