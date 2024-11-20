#pragma once
#include "utils.hpp"
#include <deque>
#include <string>

class Lexer {
public:
  explicit Lexer(const std::string &input);
  Token getNextToken();
  bool exist(TokenType type);

private:
  std::string input;
  std::deque<Token> tokens;
  Token current_token;

  void advance();
};