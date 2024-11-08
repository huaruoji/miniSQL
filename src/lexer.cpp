#include "lexer.hpp"
#include "utils.hpp"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

Lexer::Lexer(const std::string &input) : input(input), position(0) {}

Token Lexer::nextToken() {
  if (!tokenBuffer.empty()) {
    Token token = tokenBuffer.front();
    tokenBuffer.pop();
    return token;
  }

  skipWhitespace();

  if (position >= input.length()) {
    return Token(TokenType::END);
  }

  char c = peek();

  // 处理标识符和关键字
  if (isLetter(c)) {
    return parseIdentifier();
  }

  // 处理数字
  if (isDigit(c)) {
    return parseNumber();
  }

  // 处理字符串字面量
  if (c == '\'') {
    return parseString();
  }

  // 处理符号
  advance();
  switch (c) {
  case ';':
    return Token(TokenType::SEMICOLON);
  case ',':
    return Token(TokenType::COMMA);
  case '(':
    return Token(TokenType::LPAREN);
  case ')':
    return Token(TokenType::RPAREN);
  default:
    throw std::runtime_error("Unexpected character: " + std::string(1, c));
  }
}

Token Lexer::peekToken() {
  if (tokenBuffer.empty()) {
    tokenBuffer.push(nextToken());
  }
  return tokenBuffer.front();
}

bool Lexer::hasMoreTokens() const {
  return position < input.length() || !tokenBuffer.empty();
}

void Lexer::skipWhitespace() {
  while (position < input.length() && std::isspace(input[position])) {
    position++;
  }
}

char Lexer::peek() const {
  return position < input.length() ? input[position] : '\0';
}

char Lexer::advance() {
  return position < input.length() ? input[position++] : '\0';
}

bool Lexer::isDigit(char c) const { return std::isdigit(c); }

bool Lexer::isLetter(char c) const { return std::isalpha(c) || c == '_'; }

bool Lexer::isIdentifierChar(char c) const { return isLetter(c) || isDigit(c); }

Token Lexer::parseIdentifier() {
  std::string value;
  while (isIdentifierChar(peek())) {
    value += advance();
  }

  // 检查是否是关键字
  TokenType type;
  if (isKeyword(value, type)) {
    return Token(type);
  }

  return Token(TokenType::IDENTIFIER, value);
}

Token Lexer::parseNumber() {
  std::string value;
  bool isReal = false;

  while (isDigit(peek()) || peek() == '.') {
    char c = advance();
    if (c == '.') {
      if (isReal) {
        throw std::runtime_error(
            "Invalid number format: multiple decimal points");
      }
      isReal = true;
    }
    value += c;
  }

  return Token(isReal ? TokenType::REAL_LIT : TokenType::INTEGER_LIT, value);
}

Token Lexer::parseString() {
  advance(); // 跳过开始的单引号
  std::string value;

  while (peek() != '\'') {
    if (peek() == '\0') {
      throw std::runtime_error("Unterminated string literal");
    }
    value += advance();
  }

  advance(); // 跳过结束的单引号
  return Token(TokenType::STRING_LIT, value);
}

bool Lexer::isKeyword(const std::string &word, TokenType &type) {
  static const std::unordered_map<std::string, TokenType> keywords = {
      {"CREATE", TokenType::CREATE},   {"TABLE", TokenType::TABLE},
      {"DROP", TokenType::DROP},       {"INSERT", TokenType::INSERT},
      {"INTO", TokenType::INTO},       {"VALUES", TokenType::VALUES},
      {"SELECT", TokenType::SELECT},   {"FROM", TokenType::FROM},
      {"INDEX", TokenType::INDEX},     {"ON", TokenType::ON},
      {"INTEGER", TokenType::INTEGER}, {"TEXT", TokenType::TEXT},
      {"REAL", TokenType::REAL}};

  auto upperWord = utils::toUpper(word);
  auto it = keywords.find(upperWord);
  if (it != keywords.end()) {
    type = it->second;
    return true;
  }
  return false;
}