#include "lexer.hpp"

Lexer::Lexer(const std::string &input) : input(input), position(0) {}

char Lexer::peek() const {
  // TODO: Return current character without advancing
  return '\0';
}

char Lexer::advance() {
  // TODO: Move to next character and return it
  return '\0';
}

void Lexer::skipWhitespace() {
  // TODO: Skip whitespace and comments
}

Token Lexer::readIdentifier() {
  // TODO: Read and return identifier or keyword token
  return Token(TokenType::EOF_TOKEN);
}

Token Lexer::readNumber() {
  // TODO: Read and return number token
  return Token(TokenType::EOF_TOKEN);
}

Token Lexer::readString() {
  // TODO: Read and return string literal token
  return Token(TokenType::EOF_TOKEN);
}

Token Lexer::getNextToken() {
  // TODO: Implement main tokenization logic
  return Token(TokenType::EOF_TOKEN);
}