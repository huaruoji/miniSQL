#pragma once
#include <string>
#include <vector>

enum class TokenType {
  CREATE,
  DATABASE,
  USE,
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
  OR,
  INTEGER,
  FLOAT,
  TEXT,
  IDENTIFIER,
  STRING_LITERAL,
  NUMBER,
  COMMA,
  SEMICOLON,
  LEFT_PAREN,
  RIGHT_PAREN,
  EQUALS,
  GREATER_THAN,
  LESS_THAN,
  DOT,
  EOF_TOKEN
};

struct Token {
  TokenType type;
  std::string value;

  Token(TokenType t, std::string v = "") : type(t), value(v) {}
};

class Lexer {
public:
  explicit Lexer(const std::string &input);
  Token getNextToken();

private:
  std::string input;
  size_t position;

  char peek() const;
  char advance();
  void skipWhitespace();
  Token readIdentifier();
  Token readNumber();
  Token readString();
};