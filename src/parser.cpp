#include "parser.hpp"
#include "sql_statement.hpp"
#include "utils.hpp"

// 在 C++ 中,构造函数初始化列表中的初始化顺序是按照类中成员声明的顺序进行的
Parser::Parser(const std::string &input)
    : lexer(input), current_token(lexer.getNextToken()) {}

void Parser::advance() { current_token = lexer.getNextToken(); }

bool Parser::match(TokenType type) { return current_token.type == type; }

bool Parser::consume(TokenType type) {
  if (match(type)) {
    advance();
    return true;
  }
  return false;
}

std::unique_ptr<SQLStatement> Parser::parse() {
  TokenType first_token = current_token.type;
  advance();
  switch (first_token) {
  case TokenType::CREATE:
    if (consume(TokenType::DATABASE)) {
      return parseCreateDatabase();
    } else if (consume(TokenType::TABLE)) {
      return parseCreateTable();
    }
    throw ParseError("Expected DATABASE or TABLE after CREATE");

  case TokenType::USE:
    if (consume(TokenType::DATABASE)) {
      return parseUseDatabase();
    }
    throw ParseError("Expected DATABASE after USE");

  case TokenType::DROP:
    return parseDropTable();

  case TokenType::INSERT:
    return parseInsert();

  case TokenType::SELECT:
    return parseSelect();

  case TokenType::UPDATE:
    return parseUpdate();

  case TokenType::DELETE:
    return parseDelete();

  case TokenType::EOF_TOKEN:
    return nullptr;

  default:
    throw ParseError("Unexpected token at start of statement");
  }
  if (!consume(TokenType::SEMICOLON)) {
    throw ParseError("Expected SEMICOLON at end of statement");
  }
}

std::unique_ptr<WhereCondition> Parser::parseWhereCondition() {
  auto where_condition = std::make_unique<WhereCondition>();
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected column name after WHERE");
  }
  where_condition->column_name_a = current_token.value;
  if (!consume(TokenType::GREATER_THAN) && !consume(TokenType::LESS_THAN) &&
      !consume(TokenType::EQUALS)) {
    throw ParseError("Expected operator after column name");
  }
  where_condition->condition_type_a = current_token.type;
  if (!consume(TokenType::TEXT) && !consume(TokenType::INTEGER) &&
      !consume(TokenType::FLOAT)) {
    throw ParseError("Expected TEXT, INTEGER, or FLOAT after operator");
  }
  where_condition->value_a = current_token.value;
  if (consume(TokenType::AND) || consume(TokenType::OR)) {
    where_condition->logic_operator = current_token.type;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after logic operator");
    }
    where_condition->column_name_b = current_token.value;
    if (!consume(TokenType::GREATER_THAN) && !consume(TokenType::LESS_THAN) &&
        !consume(TokenType::EQUALS)) {
      throw ParseError("Expected operator after column name");
    }
    where_condition->condition_type_b = current_token.type;
    if (!consume(TokenType::TEXT) && !consume(TokenType::INTEGER) &&
        !consume(TokenType::FLOAT)) {
      throw ParseError("Expected TEXT, INTEGER, or FLOAT after operator");
    }
    where_condition->value_b = current_token.value;
  }

  return where_condition;
}

std::unique_ptr<CreateDatabaseStatement> Parser::parseCreateDatabase() {
  advance();
  if (!match(TokenType::IDENTIFIER)) {
    throw ParseError("Expected database name after CREATE DATABASE, got " +
                     current_token.value);
  }
  auto statement = std::make_unique<CreateDatabaseStatement>();
  statement->database_name = current_token.value;

  return statement;
}

std::unique_ptr<UseDatabaseStatement> Parser::parseUseDatabase() {
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected database name after USE DATABASE");
  }
  auto statement = std::make_unique<UseDatabaseStatement>();
  statement->database_name = current_token.value;

  return statement;
}

std::unique_ptr<CreateTableStatement> Parser::parseCreateTable() {
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after CREATE TABLE");
  }
  if (!consume(TokenType::LEFT_PAREN)) {
    throw ParseError("Expected LEFT_PAREN after CREATE TABLE");
  }
  auto statement = std::make_unique<CreateTableStatement>();
  statement->table_name = current_token.value;
  do {
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after LEFT_PAREN");
    }
    std::string column_name = current_token.value;
    if (!consume(TokenType::TEXT) && !consume(TokenType::INTEGER) &&
        !consume(TokenType::FLOAT)) {
      throw ParseError("Expected TEXT, INTEGER, or FLOAT after column name");
    }
    statement->columns.push_back(
        ColumnDefinition{column_name, current_token.type});
  } while (consume(TokenType::COMMA));
  if (!consume(TokenType::RIGHT_PAREN)) {
    throw ParseError("Expected RIGHT_PAREN after column definitions");
  }

  return statement;
}

std::unique_ptr<DropTableStatement> Parser::parseDropTable() {
  if (consume(TokenType::TABLE)) {
    throw ParseError("Expected TABLE after DROP");
  }
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after DROP TABLE");
  }
  auto statement = std::make_unique<DropTableStatement>();
  statement->table_name = current_token.value;

  return statement;
}

std::unique_ptr<InsertStatement> Parser::parseInsert() {
  if (!consume(TokenType::INTO)) {
    throw ParseError("Expected INTO after INSERT");
  }
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after INTO");
  }
  if (!consume(TokenType::LEFT_PAREN)) {
    throw ParseError("Expected LEFT_PAREN after INTO");
  }
  auto statement = std::make_unique<InsertStatement>();
  statement->table_name = current_token.value;
  do {
    if (!consume(TokenType::IDENTIFIER) &&
        !consume(TokenType::INTEGER_LITERAL) &&
        !consume(TokenType::FLOAT_LITERAL)) {
      throw ParseError("Expected IDENTIFIER, INTEGER_LITERAL, or FLOAT_LITERAL "
                       "after LEFT_PAREN");
    }
    statement->values.push_back(current_token.value);
  } while (consume(TokenType::COMMA));
  if (!consume(TokenType::RIGHT_PAREN)) {
    throw ParseError("Expected RIGHT_PAREN after values");
  }

  return statement;
}

std::unique_ptr<SelectStatement> Parser::parseSelect() {
  auto statement = std::make_unique<SelectStatement>();
  if (consume(TokenType::ASTERISK)) {
    statement->columns.push_back("*");
  } else {
    do {
      if (!consume(TokenType::IDENTIFIER)) {
        throw ParseError("Expected column name after SELECT");
      }
      statement->columns.push_back(current_token.value);
    } while (consume(TokenType::COMMA));
  }
  if (!consume(TokenType::FROM)) {
    throw ParseError("Expected FROM after SELECT");
  }
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after FROM");
  }
  statement->table_name = current_token.value;
  if (consume(TokenType::WHERE)) {
    statement->where_condition = parseWhereCondition();
  }

  return statement;
}

std::unique_ptr<UpdateStatement> Parser::parseUpdate() {
  auto statement = std::make_unique<UpdateStatement>();
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after UPDATE");
  }
  statement->table_name = current_token.value;
  if (!consume(TokenType::SET)) {
    throw ParseError("Expected SET after UPDATE");
  }
  do {
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after SET");
    }
    std::string column_name = current_token.value;
    if (!consume(TokenType::EQUALS)) {
      throw ParseError("Expected EQUALS after column name");
    }
    statement->set_conditions.push_back(
        SetCondition{column_name, current_token.value});
  } while (consume(TokenType::COMMA));
  if (consume(TokenType::WHERE)) {
    statement->where_condition = parseWhereCondition();
  }

  return statement;
}

std::unique_ptr<DeleteStatement> Parser::parseDelete() {
  auto statement = std::make_unique<DeleteStatement>();
  if (!consume(TokenType::FROM)) {
    throw ParseError("Expected FROM after DELETE");
  }
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after FROM");
  }
  statement->table_name = current_token.value;
  if (consume(TokenType::WHERE)) {
    statement->where_condition = parseWhereCondition();
  }

  return statement;
}