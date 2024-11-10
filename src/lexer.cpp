#include "lexer.hpp"

Lexer::Lexer(std::string input)
    : input_(std::move(input)), position_(0), current_line_(1) {}

utils::Token Lexer::nextToken() {
  // TODO: Implement lexical analysis
  // 1. Skip whitespace and comments
  // 2. Identify and return next token
  // 3. Handle different token types (keywords, identifiers, numbers, etc.)
  return utils::Token(utils::TokenType::END_OF_FILE, "", current_line_);
}

utils::Token Lexer::peekToken() {
  // TODO: Implement peek functionality
  return utils::Token(utils::TokenType::END_OF_FILE, "", current_line_);
}

int Lexer::getCurrentLine() const { return current_line_; }

void Lexer::skipWhitespace() {
  // TODO: Implement whitespace skipping
}

void Lexer::skipComment() {
  // TODO: Implement comment skipping
}

utils::Token Lexer::readNumber() {
  // TODO: Implement number reading
  return utils::Token(utils::TokenType::NUMBER, "", current_line_);
}

utils::Token Lexer::readIdentifier() {
  // TODO: Implement identifier reading
  return utils::Token(utils::TokenType::IDENTIFIER, "", current_line_);
}

utils::Token Lexer::readString() {
  // TODO: Implement string reading
  return utils::Token(utils::TokenType::STRING, "", current_line_);
}

utils::Token Lexer::readOperator() {
  // TODO: Implement operator reading
  return utils::Token(utils::TokenType::OPERATOR, "", current_line_);
}