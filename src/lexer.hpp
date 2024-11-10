#pragma once
#include "utils.hpp"
#include <string>
#include <vector>

class Lexer {
public:
  explicit Lexer(std::string input);

  // Get next token from input
  utils::Token nextToken();

  // Peek at next token without consuming it
  utils::Token peekToken();

  // Get current line number
  int getCurrentLine() const;

private:
  std::string input_;
  size_t position_;
  int current_line_;

  // Helper methods
  void skipWhitespace();
  void skipComment();
  utils::Token readNumber();
  utils::Token readIdentifier();
  utils::Token readString();
  utils::Token readOperator();
};