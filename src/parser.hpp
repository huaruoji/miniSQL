#pragma once
#include "lexer.hpp"
#include "statement.hpp"
#include "utils.hpp"
#include <memory>
#include <vector>

class Parser {
public:
  explicit Parser(const std::string &input, int start_line = 1)
      : lexer(input, start_line), current_token(lexer.getNextToken()) {}

  std::unique_ptr<SQLStatement> parse() {
    TokenType first_token = current_token.type;
    int first_token_line = current_token.line_number;
    advance();
    switch (first_token) {
    case TokenType::CREATE:
      if (consume(TokenType::DATABASE)) {
        return parseCreateDatabase();
      } else if (consume(TokenType::TABLE)) {
        return parseCreateTable();
      }
      throwError("Expected DATABASE or TABLE after CREATE", first_token_line);

    case TokenType::USE:
      if (consume(TokenType::DATABASE)) {
        return parseUseDatabase();
      }
      throwError("Expected DATABASE after USE", first_token_line);

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
      throwError("Unexpected token at start of statement", first_token_line);
    }
    
    // This line should never be reached because throwError will throw an exception
    // but we need it to satisfy the compiler
    return nullptr;
  }

private:
  Lexer lexer;
  Token current_token;

  void throwError(const std::string& message, int line_number = -1) {
    throw ParseError(message, line_number == -1 ? current_token.line_number : line_number);
  }

  void advance() { current_token = lexer.getNextToken(); }

  bool match(TokenType type) { return current_token.type == type; }

  bool consume(TokenType type) {
    if (match(type)) {
      advance();
      return true;
    }
    return false;
  }

  bool exist(TokenType type) { return lexer.exist(type); }

  std::unique_ptr<WhereCondition> parseWhereCondition() {
    auto where_condition = std::make_unique<WhereCondition>();
    
    // Parse first column name (with optional table prefix)
    std::string first_part = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throwError("Expected column name or table name after WHERE");
    }
    
    // Check if there's a dot after the identifier
    if (match(TokenType::DOT)) {
      // This is a table.column format
      consume(TokenType::DOT);
      std::string second_part = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected column name after DOT");
      }
      where_condition->column_name_a = first_part + "." + second_part;
    } else {
      // This is just a column name
      where_condition->column_name_a = first_part;
    }
    
    where_condition->condition_type_a = current_token.type;
    if (!consume(TokenType::GREATER_THAN) && !consume(TokenType::LESS_THAN) &&
        !consume(TokenType::EQUALS) && !consume(TokenType::INEQUALS)) {
      throwError("Expected operator after column name");
    }
    
    // Parse value or column reference
    first_part = current_token.value;
    if (consume(TokenType::IDENTIFIER)) {
      // Check if this is a table.column format
      if (match(TokenType::DOT)) {
        consume(TokenType::DOT);
        std::string second_part = current_token.value;
        if (!consume(TokenType::IDENTIFIER)) {
          throwError("Expected column name after DOT");
        }
        where_condition->value_a = Token(TokenType::IDENTIFIER, first_part + "." + second_part);
      } else {
        where_condition->value_a = Token(TokenType::IDENTIFIER, first_part);
      }
    } else {
      // Store the current token before consuming it
      Token value_token = current_token;
      if (consume(TokenType::STRING_LITERAL) || consume(TokenType::INTEGER_LITERAL) ||
          consume(TokenType::FLOAT_LITERAL)) {
        where_condition->value_a = value_token;
      } else {
        throwError("Expected value or column reference after operator");
      }
    }
    
    if (match(TokenType::AND) || match(TokenType::OR)) {
      where_condition->logic_operator = current_token.type;
      advance();
      
      // Parse second column name (with optional table prefix)
      first_part = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected column name or table name after logic operator");
      }
      
      // Check if there's a dot after the identifier
      if (match(TokenType::DOT)) {
        // This is a table.column format
        consume(TokenType::DOT);
        std::string second_part = current_token.value;
        if (!consume(TokenType::IDENTIFIER)) {
          throwError("Expected column name after DOT");
        }
        where_condition->column_name_b = first_part + "." + second_part;
      } else {
        // This is just a column name
        where_condition->column_name_b = first_part;
      }
      
      where_condition->condition_type_b = current_token.type;
      if (!consume(TokenType::GREATER_THAN) && !consume(TokenType::LESS_THAN) &&
          !consume(TokenType::EQUALS) && !consume(TokenType::INEQUALS)) {
        throwError("Expected operator after column name");
      }
      
      // Parse value or column reference
      first_part = current_token.value;
      if (consume(TokenType::IDENTIFIER)) {
        // Check if this is a table.column format
        if (match(TokenType::DOT)) {
          consume(TokenType::DOT);
          std::string second_part = current_token.value;
          if (!consume(TokenType::IDENTIFIER)) {
            throwError("Expected column name after DOT");
          }
          where_condition->value_b = Token(TokenType::IDENTIFIER, first_part + "." + second_part);
        } else {
          where_condition->value_b = Token(TokenType::IDENTIFIER, first_part);
        }
      } else {
        // Store the current token before consuming it
        Token value_token = current_token;
        if (consume(TokenType::STRING_LITERAL) || consume(TokenType::INTEGER_LITERAL) ||
            consume(TokenType::FLOAT_LITERAL)) {
          where_condition->value_b = value_token;
        } else {
          throwError("Expected value or column reference after operator");
        }
      }
    }

    return where_condition;
  }

  std::unique_ptr<CreateDatabaseStatement> parseCreateDatabase() {
    auto stmt = std::make_unique<CreateDatabaseStatement>();
    stmt->line_number = current_token.line_number;
    stmt->database_name = current_token.value;
    advance();
    return stmt;
  }

  std::unique_ptr<UseDatabaseStatement> parseUseDatabase() {
    auto stmt = std::make_unique<UseDatabaseStatement>();
    stmt->line_number = current_token.line_number;
    stmt->database_name = current_token.value;
    advance();
    return stmt;
  }

  std::unique_ptr<CreateTableStatement> parseCreateTable() {
    auto stmt = std::make_unique<CreateTableStatement>();
    stmt->line_number = current_token.line_number;
    stmt->table_name = current_token.value;
    advance();
    if (!consume(TokenType::LEFT_PAREN)) {
      throwError("Expected LEFT_PAREN after CREATE TABLE");
    }
    do {
      std::string column_name = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected column name after LEFT_PAREN");
      }
      TokenType token_type = current_token.type;
      if (!consume(TokenType::TEXT) && !consume(TokenType::INTEGER) &&
          !consume(TokenType::FLOAT)) {
        throwError("Expected TEXT, INTEGER, or FLOAT after column name");
      }
      stmt->columns.push_back(ColumnDefinition{column_name, token_type});
    } while (consume(TokenType::COMMA));
    if (!consume(TokenType::RIGHT_PAREN)) {
      throwError("Expected RIGHT_PAREN after column definitions");
    }

    return stmt;
  }

  std::unique_ptr<DropTableStatement> parseDropTable() {
    auto statement = std::make_unique<DropTableStatement>();
    statement->line_number = current_token.line_number;
    if (!consume(TokenType::TABLE)) {
      throwError("Expected TABLE after DROP");
    }
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throwError("Expected table name after DROP TABLE");
    }

    return statement;
  }

  std::unique_ptr<InsertStatement> parseInsert() {
    auto statement = std::make_unique<InsertStatement>();
    statement->line_number = current_token.line_number;
    if (!consume(TokenType::INTO)) {
      throwError("Expected INTO after INSERT");
    }
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throwError("Expected table name after INTO");
    }
    if (!consume(TokenType::VALUES)) {
      throwError("Expected VALUES after table name");
    }
    if (!consume(TokenType::LEFT_PAREN)) {
      throwError("Expected LEFT_PAREN after INTO");
    }
    do {
      statement->values.push_back(convertTokenToValue(current_token));
      if (!(consume(TokenType::STRING_LITERAL) ||
            consume(TokenType::INTEGER_LITERAL) ||
            consume(TokenType::FLOAT_LITERAL))) {
        throwError(
            "Expected STRING_LITERAL, INTEGER_LITERAL, or FLOAT_LITERAL "
            "after LEFT_PAREN, got " +
                TOKEN_STR.find(current_token.type)->second + ": " +
                current_token.value);
      }
    } while (consume(TokenType::COMMA));
    if (!consume(TokenType::RIGHT_PAREN)) {
      throwError("Expected RIGHT_PAREN after values");
    }

    return statement;
  }

  std::unique_ptr<SelectStatement> parseSelect() {
    auto statement = std::make_unique<SelectStatement>();
    statement->line_number = current_token.line_number;
    consume(TokenType::ASTERISK);
    if (!match(TokenType::FROM)) {
      do {
        statement->columns.push_back(current_token.value);
        if (!consume(TokenType::IDENTIFIER)) {
          throwError("Expected column name after SELECT");
        }
      } while (consume(TokenType::COMMA));
    }
    if (!consume(TokenType::FROM)) {
      throwError("Expected FROM after SELECT");
    }
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throwError("Expected table name after FROM");
    }
    if (consume(TokenType::WHERE)) {
      statement->where_condition = parseWhereCondition();
    }

    return statement;
  }

  std::unique_ptr<UpdateStatement> parseUpdate() {
    auto statement = std::make_unique<UpdateStatement>();
    statement->line_number = current_token.line_number;
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throwError("Expected table name after UPDATE");
    }
    if (!consume(TokenType::SET)) {
      throwError("Expected SET after UPDATE");
    }
    do {
      std::string column_name_a = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected column name after SET");
      }
      if (!consume(TokenType::EQUALS)) {
        throwError("Expected EQUALS after column name");
      }
      TokenType condition_type = TokenType::EOF_TOKEN;
      Token value = current_token;
      std::string column_name_b;
      if (match(TokenType::IDENTIFIER)) {
        column_name_b = current_token.value;
        advance();
        condition_type = current_token.type;
        if (!(consume(TokenType::PLUS) || consume(TokenType::MINUS))) {
          throwError("Expected PLUS or MINUS after column name");
        }
        value = current_token;
        if (!(consume(TokenType::INTEGER_LITERAL) ||
              consume(TokenType::FLOAT_LITERAL))) {
          throwError(
              "Expected INTEGER_LITERAL or FLOAT_LITERAL after PLUS or MINUS");
        }
      } else if (!(consume(TokenType::STRING_LITERAL) ||
                   consume(TokenType::INTEGER_LITERAL) ||
                   consume(TokenType::FLOAT_LITERAL))) {
        throwError("Expected STRING_LITERAL, INTEGER_LITERAL, "
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

  std::unique_ptr<DeleteStatement> parseDelete() {
    auto statement = std::make_unique<DeleteStatement>();
    statement->line_number = current_token.line_number;
    if (!consume(TokenType::FROM)) {
      throwError("Expected FROM after DELETE");
    }
    statement->table_name = current_token.value;
    if (!consume(TokenType::IDENTIFIER)) {
      throwError("Expected table name after FROM");
    }
    if (consume(TokenType::WHERE)) {
      statement->where_condition = parseWhereCondition();
    }

    return statement;
  }

  std::unique_ptr<InnerJoinStatement> parseInnerJoin() {
    auto statement = std::make_unique<InnerJoinStatement>();
    statement->line_number = current_token.line_number;

    // Parse selected columns
    do {
      std::string table_name = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected table name after SELECT");
      }
      if (!consume(TokenType::DOT)) {
        throwError("Expected DOT after table name");
      }
      std::string column_name = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected column name after DOT");
      }
      statement->selected_columns.push_back(table_name + "." + column_name);
    } while (consume(TokenType::COMMA));

    // Parse FROM clause
    if (!consume(TokenType::FROM)) {
      throwError("Expected FROM after column list");
    }

    // Parse first table
    statement->tables.push_back(current_token.value);
    if (!consume(TokenType::IDENTIFIER)) {
      throwError("Expected table name after FROM");
    }

    // Parse INNER JOIN clauses
    while (consume(TokenType::INNER)) {
      if (!consume(TokenType::JOIN)) {
        throwError("Expected JOIN after INNER");
      }

      // Get the next table name
      statement->tables.push_back(current_token.value);
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected table name after JOIN");
      }

      // Parse ON condition
      if (!consume(TokenType::ON)) {
        throwError("Expected ON after table name");
      }

      // Parse join condition
      std::string table_name_a = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected table name");
      }
      if (!consume(TokenType::DOT)) {
        throwError("Expected DOT after table name");
      }
      std::string column_name_a = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected column name after DOT");
      }

      // Parse operator
      TokenType operator_type = current_token.type;
      if (!(consume(TokenType::EQUALS) || consume(TokenType::GREATER_THAN) ||
            consume(TokenType::LESS_THAN) || consume(TokenType::INEQUALS))) {
        throwError("Expected comparison operator");
      }
      statement->join_operators.push_back(operator_type);

      // Parse second part of condition
      std::string table_name_b = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected table name");
      }
      if (!consume(TokenType::DOT)) {
        throwError("Expected DOT after table name");
      }
      std::string column_name_b = current_token.value;
      if (!consume(TokenType::IDENTIFIER)) {
        throwError("Expected column name after DOT");
      }

      // Add join condition
      statement->join_conditions.emplace_back(
          table_name_a + "." + column_name_a,
          table_name_b + "." + column_name_b);
    }

    // Parse optional WHERE clause
    if (consume(TokenType::WHERE)) {
      statement->where_condition = parseWhereCondition();
    }

    return statement;
  }
};