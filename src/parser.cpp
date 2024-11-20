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

bool Parser::exist(TokenType type) {
  return lexer.exist(type) || current_token.type == type;
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
    if (exist(TokenType::INNER)) {
      return parseInnerJoin();
    }
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
  where_condition->column_name_a = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected column name after WHERE");
  }
  where_condition->condition_type_a = current_token.type;
  if (!consume(TokenType::GREATER_THAN) && !consume(TokenType::LESS_THAN) &&
      !consume(TokenType::EQUALS)) {
    throw ParseError("Expected operator after column name");
  }
  where_condition->value_a = current_token.value;
  if (!(consume(TokenType::STRING_LITERAL) ||
        consume(TokenType::INTEGER_LITERAL) ||
        consume(TokenType::FLOAT_LITERAL))) {
    throw ParseError(
        "Expected STRING_LITERAL, INTEGER_LITERAL, or FLOAT_LITERAL "
        "after operator");
  }
  where_condition->logic_operator = current_token.type;
  if (consume(TokenType::AND) || consume(TokenType::OR)) {
    where_condition->column_name_b = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after logic operator");
    }
    where_condition->condition_type_b = current_token.type;
    if (!consume(TokenType::GREATER_THAN) && !consume(TokenType::LESS_THAN) &&
        !consume(TokenType::EQUALS)) {
      throw ParseError("Expected operator after column name");
    }
    where_condition->value_b = current_token.value;
    if (!(consume(TokenType::STRING_LITERAL) ||
          consume(TokenType::INTEGER_LITERAL) ||
          consume(TokenType::FLOAT_LITERAL))) {
      throw ParseError(
          "Expected STRING_LITERAL, INTEGER_LITERAL, or FLOAT_LITERAL "
          "after operator");
    }
  }

  return where_condition;
}

std::unique_ptr<CreateDatabaseStatement> Parser::parseCreateDatabase() {
  auto statement = std::make_unique<CreateDatabaseStatement>();
  statement->database_name = current_token.value;
  if (!match(TokenType::IDENTIFIER)) {
    throw ParseError("Expected database name after CREATE DATABASE, got " +
                     current_token.value);
  }

  return statement;
}

std::unique_ptr<UseDatabaseStatement> Parser::parseUseDatabase() {
  auto statement = std::make_unique<UseDatabaseStatement>();
  statement->database_name = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected database name after USE DATABASE");
  }

  return statement;
}

std::unique_ptr<CreateTableStatement> Parser::parseCreateTable() {
  auto statement = std::make_unique<CreateTableStatement>();
  statement->table_name = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after CREATE TABLE");
  }
  if (!consume(TokenType::LEFT_PAREN)) {
    throw ParseError("Expected LEFT_PAREN after CREATE TABLE");
  }
  do {
    std::string column_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after LEFT_PAREN");
    }
    TokenType token_type = current_token.type;
    if (!consume(TokenType::TEXT) && !consume(TokenType::INTEGER) &&
        !consume(TokenType::FLOAT)) {
      throw ParseError("Expected TEXT, INTEGER, or FLOAT after column name");
    }
    statement->columns.push_back(ColumnDefinition{column_name, token_type});
  } while (consume(TokenType::COMMA));
  if (!consume(TokenType::RIGHT_PAREN)) {
    throw ParseError("Expected RIGHT_PAREN after column definitions");
  }

  return statement;
}

std::unique_ptr<DropTableStatement> Parser::parseDropTable() {
  auto statement = std::make_unique<DropTableStatement>();
  if (!consume(TokenType::TABLE)) {
    throw ParseError("Expected TABLE after DROP");
  }
  statement->table_name = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after DROP TABLE");
  }

  return statement;
}

std::unique_ptr<InsertStatement> Parser::parseInsert() {
  auto statement = std::make_unique<InsertStatement>();
  if (!consume(TokenType::INTO)) {
    throw ParseError("Expected INTO after INSERT");
  }
  statement->table_name = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after INTO");
  }
  if (!consume(TokenType::VALUES)) {
    throw ParseError("Expected VALUES after table name");
  }
  if (!consume(TokenType::LEFT_PAREN)) {
    throw ParseError("Expected LEFT_PAREN after INTO");
  }
  do {
    statement->values.push_back(current_token.value);
    std::cerr << current_token.value << std::endl;
    if (!(consume(TokenType::STRING_LITERAL) ||
          consume(TokenType::INTEGER_LITERAL) ||
          consume(TokenType::FLOAT_LITERAL))) {
      throw ParseError(
          "Expected STRING_LITERAL, INTEGER_LITERAL, or FLOAT_LITERAL "
          "after LEFT_PAREN, got " +
          TOKEN_STR.find(current_token.type)->second + ": " +
          current_token.value);
    }
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
      statement->columns.push_back(current_token.value);
      if (!consume(TokenType::IDENTIFIER)) {
        throw ParseError("Expected column name after SELECT");
      }
    } while (consume(TokenType::COMMA));
  }
  if (!consume(TokenType::FROM)) {
    throw ParseError("Expected FROM after SELECT");
  }
  statement->table_name = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after FROM");
  }
  if (consume(TokenType::WHERE)) {
    statement->where_condition = parseWhereCondition();
  }

  return statement;
}

std::unique_ptr<UpdateStatement> Parser::parseUpdate() {
  auto statement = std::make_unique<UpdateStatement>();
  statement->table_name = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after UPDATE");
  }
  if (!consume(TokenType::SET)) {
    throw ParseError("Expected SET after UPDATE");
  }
  do {
    std::string column_name_a = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after SET");
    }
    if (!consume(TokenType::EQUALS)) {
      throw ParseError("Expected EQUALS after column name");
    }
    TokenType condition_type = TokenType::EOF_TOKEN;
    Value value;
    std::string column_name_b = current_token.value;
    if (consume(TokenType::IDENTIFIER)) {
      condition_type = current_token.type;
      if (!(consume(TokenType::PLUS) || consume(TokenType::MINUS))) {
        throw ParseError("Expected PLUS or MINUS after column name");
      }
      value = current_token.value;
      if (!(consume(TokenType::INTEGER_LITERAL) ||
            consume(TokenType::FLOAT_LITERAL))) {
        throw ParseError(
            "Expected INTEGER_LITERAL or FLOAT_LITERAL after PLUS or MINUS");
      }
    } else if (!(consume(TokenType::STRING_LITERAL) ||
                 consume(TokenType::INTEGER_LITERAL) ||
                 consume(TokenType::FLOAT_LITERAL))) {
      throw ParseError("Expected STRING_LITERAL, INTEGER_LITERAL, "
                       "FLOAT_LITERAL, or column name "
                       "after equals sign");
    }
    statement->set_conditions.push_back(
        SetCondition{column_name_a, column_name_b, condition_type, value});
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
  statement->table_name = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after FROM");
  }
  if (consume(TokenType::WHERE)) {
    statement->where_condition = parseWhereCondition();
  }

  return statement;
}

std::unique_ptr<InnerJoinStatement> Parser::parseInnerJoin() {
  auto statement = std::make_unique<InnerJoinStatement>();
  statement->table_name_a = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after SELECT");
  }
  if (!consume(TokenType::DOT)) {
    throw ParseError("Expected DOT after table name");
  }
  statement->column_name_a = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected column name after DOT");
  }
  if (!consume(TokenType::COMMA)) {
    throw ParseError("Expected COMMA after column name");
  }
  statement->table_name_b = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after SELECT");
  }
  if (!consume(TokenType::DOT)) {
    throw ParseError("Expected DOT after table name");
  }
  statement->column_name_b = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected column name after DOT");
  }
  if (!consume(TokenType::FROM)) {
    throw ParseError("Expected FROM after column name");
  }
  if (current_token.value != statement->table_name_a) {
    throw ParseError("Expected table name to be " + statement->table_name_a);
  }
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after FROM");
  }
  if (!consume(TokenType::INNER)) {
    throw ParseError("Expected INNER after table name");
  }
  if (!consume(TokenType::JOIN)) {
    throw ParseError("Expected JOIN after INNER");
  }
  if (current_token.value != statement->table_name_b) {
    throw ParseError("Expected table name to be " + statement->table_name_b);
  }
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after JOIN");
  }
  if (!consume(TokenType::ON)) {
    throw ParseError("Expected ON after JOIN");
  }
  if (current_token.value != statement->table_name_a) {
    throw ParseError("Expected table name to be " + statement->table_name_a);
  }
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected table name after ON");
  }
  if (!consume(TokenType::DOT)) {
    throw ParseError("Expected DOT after column name");
  }
  statement->condition_column_name_a = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected column name after DOT");
  }
  statement->condition_type = current_token.type;
  if (!(consume(TokenType::EQUALS) || consume(TokenType::GREATER_THAN) ||
        consume(TokenType::LESS_THAN))) {
    throw ParseError("Expected EQUALS, GREATER_THAN, or LESS_THAN after "
                     "column name");
  }
  if (current_token.value != statement->table_name_b) {
    throw ParseError("Expected table name to be " + statement->table_name_b);
  }
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected column name after EQUALS");
  }
  if (!consume(TokenType::DOT)) {
    throw ParseError("Expected DOT after column name");
  }
  statement->condition_column_name_b = current_token.value;
  if (!consume(TokenType::IDENTIFIER)) {
    throw ParseError("Expected column name after DOT");
  }

  return statement;
}