#pragma once
#include <string>
#include <vector>

enum class TokenType {
  // 关键字
  CREATE,
  USE,
  DATABASE,
  TABLE,
  DROP,
  INSERT,
  INTO,
  VALUES,
  SELECT,
  FROM,
  WHERE,
  UPDATE,
  SET,
  DELETE,
  INNER,
  JOIN,
  ON,
  AND,

  // 数据类型
  INTEGER,
  TEXT,
  FLOAT,

  // 符号
  SEMICOLON, // ;
  COMMA,     // ,
  LPAREN,    // (
  RPAREN,    // )
  ASTERISK,  // *
  EQUALS,    // =
  GT,        // >
  GTE,       // >=
  LT,        // <
  LTE,       // <=
  DOT,       // .

  // 其他
  IDENTIFIER,
  INTEGER_LIT,
  FLOAT_LIT,
  STRING_LIT,
  END
};

struct Token {
  TokenType type;
  std::string value;
  size_t line;
  size_t column;

  Token(TokenType t, const std::string &v = "", size_t l = 1, size_t c = 1)
      : type(t), value(v), line(l), column(c) {}
};

class Lexer {
public:
  explicit Lexer(const std::string &input);
  Token nextToken();
  bool hasMoreTokens() const { return position < input.length(); }

private:
  std::string input;
  size_t position{0};
  size_t line{1};
  size_t column{1};

  void skipWhitespaceAndComments();
  Token parseIdentifierOrKeyword();
  Token parseNumber();
  Token parseString();

  char peek() const {
    return position < input.length() ? input[position] : '\0';
  }
  char advance();

  void updatePosition(char c) {
    if (c == '\n') {
      line++;
      column = 1;
    } else {
      column++;
    }
  }
};