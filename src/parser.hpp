#pragma once
#include "lexer.hpp"
#include "statement.hpp"
#include "utils.hpp"
#include <memory>
#include <vector>

class Parser {
public:
  explicit Parser(const std::string &input, int start_line = 1)
      : lexer(input), current_token(lexer.getNextToken()),
        statement_start_line(start_line) {}

  std::unique_ptr<SQLStatement> parse() {
    TokenType first_token = current_token.type;
    int first_token_line = current_token.line_number + statement_start_line - 1;
    advance();
    switch (first_token) {
    case TokenType::CREATE:
      if (consume(TokenType::DATABASE)) {
        return parseCreateDatabase();
      } else if (consume(TokenType::TABLE)) {
        return parseCreateTable();
      }
      throw ParseError("Expected DATABASE or TABLE after CREATE",
                       first_token_line);

    case TokenType::USE:
      if (consume(TokenType::DATABASE)) {
        return parseUseDatabase();
      }
      throw ParseError("Expected DATABASE after USE", first_token_line);

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
      throw ParseError("Unexpected token at start of statement",
                       first_token_line);
    }
    if (!consume(TokenType::SEMICOLON)) {
      throw ParseError("Expected SEMICOLON at end of statement");
    }
  }

private:
  Lexer lexer;
  Token current_token;
  int statement_start_line;

  void advance() { current_token = lexer.getNextToken(); }

  bool match(TokenType type) { return current_token.type == type; }

  bool consume(TokenType type) {
    if (match(type)) {
      advance();
      return true;
    }
    return false;
  }

  bool exist(TokenType type) {
    return lexer.exist(type) || current_token.type == type;
  }

  std::unique_ptr<WhereCondition> parseWhereCondition() {
    auto where_condition = std::make_unique<WhereCondition>();
    where_condition->column_name_a = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after WHERE",
                       current_token.line_number + statement_start_line - 1);
    }
    where_condition->condition_type_a = current_token.type;
    if (!consume(TokenType::GREATER_THAN) && !consume(TokenType::LESS_THAN) &&
        !consume(TokenType::EQUALS)) {
      throw ParseError("Expected operator after column name",
                       current_token.line_number + statement_start_line - 1);
    }
    where_condition->value_a = current_token;
    if (!(consume(TokenType::STRING_LITERAL) ||
          consume(TokenType::INTEGER_LITERAL) ||
          consume(TokenType::FLOAT_LITERAL))) {
      throw ParseError(
          "Expected STRING_LITERAL, INTEGER_LITERAL, or FLOAT_LITERAL "
          "after operator",
          current_token.line_number + statement_start_line - 1);
    }
    if (match(TokenType::AND) || match(TokenType::OR)) {
      where_condition->logic_operator = current_token.type;
      advance();
      where_condition->column_name_b = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throw ParseError("Expected column name after logic operator",
                         current_token.line_number + statement_start_line - 1);
      }
      where_condition->condition_type_b = current_token.type;
      if (!consume(TokenType::GREATER_THAN) && !consume(TokenType::LESS_THAN) &&
          !consume(TokenType::EQUALS)) {
        throw ParseError("Expected operator after column name",
                         current_token.line_number + statement_start_line - 1);
      }
      where_condition->value_b = current_token;
      if (!(consume(TokenType::STRING_LITERAL) ||
            consume(TokenType::INTEGER_LITERAL) ||
            consume(TokenType::FLOAT_LITERAL))) {
        throw ParseError(
            "Expected STRING_LITERAL, INTEGER_LITERAL, or FLOAT_LITERAL "
            "after operator",
            current_token.line_number + statement_start_line - 1);
      }
    }

    return where_condition;
  }

  std::unique_ptr<CreateDatabaseStatement> parseCreateDatabase() {
    auto stmt = std::make_unique<CreateDatabaseStatement>();
    stmt->line_number = current_token.line_number + statement_start_line - 1;
    stmt->database_name = current_token.value;
    advance();
    return stmt;
  }

  std::unique_ptr<UseDatabaseStatement> parseUseDatabase() {
    auto stmt = std::make_unique<UseDatabaseStatement>();
    stmt->line_number = current_token.line_number + statement_start_line - 1;
    stmt->database_name = current_token.value;
    advance();
    return stmt;
  }

  std::unique_ptr<CreateTableStatement> parseCreateTable() {
    auto stmt = std::make_unique<CreateTableStatement>();
    stmt->line_number = current_token.line_number + statement_start_line - 1;
    stmt->table_name = current_token.value;
    advance();
    if (!consume(TokenType::LEFT_PAREN)) {
      throw ParseError("Expected LEFT_PAREN after CREATE TABLE",
                       current_token.line_number + statement_start_line - 1);
    }
    do {
      std::string column_name = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throw ParseError("Expected column name after LEFT_PAREN",
                         current_token.line_number + statement_start_line - 1);
      }
      TokenType token_type = current_token.type;
      if (!consume(TokenType::TEXT) && !consume(TokenType::INTEGER) &&
          !consume(TokenType::FLOAT)) {
        throw ParseError("Expected TEXT, INTEGER, or FLOAT after column name",
                         current_token.line_number + statement_start_line - 1);
      }
      stmt->columns.push_back(ColumnDefinition{column_name, token_type});
    } while (consume(TokenType::COMMA));
    if (!consume(TokenType::RIGHT_PAREN)) {
      throw ParseError("Expected RIGHT_PAREN after column definitions",
                       current_token.line_number + statement_start_line - 1);
    }

    return stmt;
  }

  std::unique_ptr<DropTableStatement> parseDropTable() {
    auto statement = std::make_unique<DropTableStatement>();
    statement->line_number =
        current_token.line_number + statement_start_line - 1;
    if (!consume(TokenType::TABLE)) {
      throw ParseError("Expected TABLE after DROP",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after DROP TABLE",
                       current_token.line_number + statement_start_line - 1);
    }

    return statement;
  }

  std::unique_ptr<InsertStatement> parseInsert() {
    auto statement = std::make_unique<InsertStatement>();
    statement->line_number =
        current_token.line_number + statement_start_line - 1;
    if (!consume(TokenType::INTO)) {
      throw ParseError("Expected INTO after INSERT",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after INTO",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::VALUES)) {
      throw ParseError("Expected VALUES after table name",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::LEFT_PAREN)) {
      throw ParseError("Expected LEFT_PAREN after INTO",
                       current_token.line_number + statement_start_line - 1);
    }
    do {
      statement->values.push_back(convertTokenToValue(current_token));
      if (!(consume(TokenType::STRING_LITERAL) ||
            consume(TokenType::INTEGER_LITERAL) ||
            consume(TokenType::FLOAT_LITERAL))) {
        throw ParseError(
            "Expected STRING_LITERAL, INTEGER_LITERAL, or FLOAT_LITERAL "
            "after LEFT_PAREN, got " +
                TOKEN_STR.find(current_token.type)->second + ": " +
                current_token.value,
            current_token.line_number + statement_start_line - 1);
      }
    } while (consume(TokenType::COMMA));
    if (!consume(TokenType::RIGHT_PAREN)) {
      throw ParseError("Expected RIGHT_PAREN after values",
                       current_token.line_number + statement_start_line - 1);
    }

    return statement;
  }

  std::unique_ptr<SelectStatement> parseSelect() {
    auto statement = std::make_unique<SelectStatement>();
    statement->line_number =
        current_token.line_number + statement_start_line - 1;
    consume(TokenType::ASTERISK);
    if (!match(TokenType::FROM)) {
      do {
        statement->columns.push_back(current_token.value);
        if (!consume(TokenType::IDENTIFIER)) {
          throw ParseError("Expected column name after SELECT",
                           current_token.line_number + statement_start_line -
                               1);
        }
      } while (consume(TokenType::COMMA));
    }
    if (!consume(TokenType::FROM)) {
      throw ParseError("Expected FROM after SELECT",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after FROM",
                       current_token.line_number + statement_start_line - 1);
    }
    if (consume(TokenType::WHERE)) {
      statement->where_condition = parseWhereCondition();
    }

    return statement;
  }

  std::unique_ptr<UpdateStatement> parseUpdate() {
    auto statement = std::make_unique<UpdateStatement>();
    statement->line_number =
        current_token.line_number + statement_start_line - 1;
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after UPDATE",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::SET)) {
      throw ParseError("Expected SET after UPDATE",
                       current_token.line_number + statement_start_line - 1);
    }
    do {
      std::string column_name_a = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throw ParseError("Expected column name after SET",
                         current_token.line_number + statement_start_line - 1);
      }
      if (!consume(TokenType::EQUALS)) {
        throw ParseError("Expected EQUALS after column name",
                         current_token.line_number + statement_start_line - 1);
      }
      TokenType condition_type = TokenType::EOF_TOKEN;
      Token value = current_token;
      std::string column_name_b;
      if (match(TokenType::IDENTIFIER)) {
        column_name_b = current_token.value;
        advance();
        condition_type = current_token.type;
        if (!(consume(TokenType::PLUS) || consume(TokenType::MINUS))) {
          throw ParseError("Expected PLUS or MINUS after column name",
                           current_token.line_number + statement_start_line -
                               1);
        }
        value = current_token;
        if (!(consume(TokenType::INTEGER_LITERAL) ||
              consume(TokenType::FLOAT_LITERAL))) {
          throw ParseError(
              "Expected INTEGER_LITERAL or FLOAT_LITERAL after PLUS or MINUS",
              current_token.line_number + statement_start_line - 1);
        }
      } else if (!(consume(TokenType::STRING_LITERAL) ||
                   consume(TokenType::INTEGER_LITERAL) ||
                   consume(TokenType::FLOAT_LITERAL))) {
        throw ParseError("Expected STRING_LITERAL, INTEGER_LITERAL, "
                         "FLOAT_LITERAL, or column name "
                         "after equals sign",
                         current_token.line_number + statement_start_line - 1);
      }
      statement->set_conditions.push_back(
          SetCondition{column_name_a, column_name_b, condition_type, value});
    } while (consume(TokenType::COMMA));
    if (consume(TokenType::WHERE)) {
      statement->where_condition = parseWhereCondition();
    }

    return statement;
  }

  std::unique_ptr<DeleteStatement> parseDelete() {
    auto statement = std::make_unique<DeleteStatement>();
    statement->line_number =
        current_token.line_number + statement_start_line - 1;
    if (!consume(TokenType::FROM)) {
      throw ParseError("Expected FROM after DELETE",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after FROM",
                       current_token.line_number + statement_start_line - 1);
    }
    if (consume(TokenType::WHERE)) {
      statement->where_condition = parseWhereCondition();
    }

    return statement;
  }

  std::unique_ptr<InnerJoinStatement> parseInnerJoin() {
    auto statement = std::make_unique<InnerJoinStatement>();
    statement->line_number =
        current_token.line_number + statement_start_line - 1;
    statement->table_name_a = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after SELECT",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::DOT)) {
      throw ParseError("Expected DOT after table name",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->column_name_a = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after DOT",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::COMMA)) {
      throw ParseError("Expected COMMA after column name",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->table_name_b = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after SELECT",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::DOT)) {
      throw ParseError("Expected DOT after table name",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->column_name_b = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after DOT",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::FROM)) {
      throw ParseError("Expected FROM after column name",
                       current_token.line_number + statement_start_line - 1);
    }
    if (current_token.value != statement->table_name_a) {
      throw ParseError("Expected table name to be " + statement->table_name_a,
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after FROM",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::INNER)) {
      throw ParseError("Expected INNER after table name",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::JOIN)) {
      throw ParseError("Expected JOIN after INNER",
                       current_token.line_number + statement_start_line - 1);
    }
    if (current_token.value != statement->table_name_b) {
      throw ParseError("Expected table name to be " + statement->table_name_b,
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after JOIN",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::ON)) {
      throw ParseError("Expected ON after JOIN",
                       current_token.line_number + statement_start_line - 1);
    }
    if (current_token.value != statement->table_name_a) {
      throw ParseError("Expected table name to be " + statement->table_name_a,
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected table name after ON",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::DOT)) {
      throw ParseError("Expected DOT after column name",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->condition_column_name_a = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after DOT",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->condition_type = current_token.type;
    if (!(consume(TokenType::EQUALS) || consume(TokenType::GREATER_THAN) ||
          consume(TokenType::LESS_THAN))) {
      throw ParseError("Expected EQUALS, GREATER_THAN, or LESS_THAN after "
                       "column name",
                       current_token.line_number + statement_start_line - 1);
    }
    if (current_token.value != statement->table_name_b) {
      throw ParseError("Expected table name to be " + statement->table_name_b,
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after EQUALS",
                       current_token.line_number + statement_start_line - 1);
    }
    if (!consume(TokenType::DOT)) {
      throw ParseError("Expected DOT after column name",
                       current_token.line_number + statement_start_line - 1);
    }
    statement->condition_column_name_b = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throw ParseError("Expected column name after DOT",
                       current_token.line_number + statement_start_line - 1);
    }

    return statement;
  }
};