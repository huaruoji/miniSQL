#pragma once
#include "utils.hpp"
#include <deque>
#include <string>

class Lexer {
public:
  explicit Lexer(const std::string &input);
  Token getNextToken();

private:
  std::string input;
  std::deque<Token> tokens;
  Token current_token;

  void advance();
  Token recognizeToken(const std::string &token);
};