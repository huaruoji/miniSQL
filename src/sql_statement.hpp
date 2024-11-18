#pragma once

#include "utils.hpp"
#include <memory>
#include <string>
#include <vector>

struct SQLStatement {
  SQLStatementType type;
  virtual ~SQLStatement() = default;
  virtual std::string getDatabaseName() const { return ""; }
};

// SQL statement types

struct CreateDatabaseStatement : SQLStatement {
  std::string database_name;
  std::string getDatabaseName() const override { return database_name; }
};

struct UseDatabaseStatement : SQLStatement {
  std::string database_name;
  std::string getDatabaseName() const override { return database_name; }
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
  std::vector<Value> values;
};

struct SelectStatement : SQLStatement {
  std::vector<std::string> columns;
  std::string table_name;
  std::unique_ptr<WhereCondition> where_condition;
};

struct InnerJoinStatement : SQLStatement {
  std::string table_name_a;
  std::string table_name_b;
  std::string column_name_a;
  std::string column_name_b;
  std::string condition_column_name_a;
  std::string condition_column_name_b;
  TokenType condition_type;
};

struct UpdateStatement : SQLStatement {
  std::string table_name;
  std::vector<SetCondition> set_conditions;
  std::unique_ptr<WhereCondition> where_condition;
};

struct DeleteStatement : SQLStatement {
  std::string table_name;
  std::unique_ptr<WhereCondition> where_condition;
};