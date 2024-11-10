#include "lexer.hpp"
#include "utils.hpp"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

namespace {
const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"CREATE", TokenType::CREATE},
    {"USE", TokenType::USE},
    {"DATABASE", TokenType::DATABASE},
    {"TABLE", TokenType::TABLE},
    {"DROP", TokenType::DROP},
    {"INSERT", TokenType::INSERT},
    {"INTO", TokenType::INTO},
    {"VALUES", TokenType::VALUES},
    {"SELECT", TokenType::SELECT},
    {"FROM", TokenType::FROM},
    {"WHERE", TokenType::WHERE},
    {"UPDATE", TokenType::UPDATE},
    {"SET", TokenType::SET},
    {"DELETE", TokenType::DELETE},
    {"INNER", TokenType::INNER},
    {"JOIN", TokenType::JOIN},
    {"ON", TokenType::ON},
    {"AND", TokenType::AND},
    {"INTEGER", TokenType::INTEGER},
    {"TEXT", TokenType::TEXT},
    {"FLOAT", TokenType::FLOAT}};
}

Lexer::Lexer(const std::string &input) : input(input) {}

char Lexer::advance() {
  char c = peek();
  if (c != '\0') {
    updatePosition(c);
    position++;
  }
  return c;
}

void Lexer::skipWhitespaceAndComments() {
  while (position < input.length()) {
    // 处理注释
    if (peek() == '-' && position + 1 < input.length() &&
        input[position + 1] == '-') {
      while (position < input.length() && peek() != '\n') {
        advance();
      }
      if (peek() == '\n')
        advance();
      continue;
    }

    // 处理空白字符
    if (std::isspace(peek())) {
      advance();
      continue;
    }

    break;
  }
}

Token Lexer::nextToken() {
  skipWhitespaceAndComments();

  if (position >= input.length()) {
    return Token(TokenType::END, "", line, column);
  }

  size_t start_column = column;
  char c = peek();

  // 标识符或关键字
  if (std::isalpha(c) || c == '_') {
    return parseIdentifierOrKeyword();
  }

  // 数字
  if (std::isdigit(c)) {
    return parseNumber();
  }

  // 字符串
  if (c == '\'') {
    return parseString();
  }

  // 单字符标记
  advance();
  switch (c) {
  case ';':
    return Token(TokenType::SEMICOLON, ";", line, start_column);
  case ',':
    return Token(TokenType::COMMA, ",", line, start_column);
  case '(':
    return Token(TokenType::LPAREN, "(", line, start_column);
  case ')':
    return Token(TokenType::RPAREN, ")", line, start_column);
  case '*':
    return Token(TokenType::ASTERISK, "*", line, start_column);
  case '=':
    return Token(TokenType::EQUALS, "=", line, start_column);
  case '>':
    if (peek() == '=') {
      advance();
      return Token(TokenType::GTE, ">=", line, start_column);
    }
    return Token(TokenType::GT, ">", line, start_column);
  case '<':
    if (peek() == '=') {
      advance();
      return Token(TokenType::LTE, "<=", line, start_column);
    }
    return Token(TokenType::LT, "<", line, start_column);
  case '.':
    return Token(TokenType::DOT, ".", line, start_column);
  default:
    std::string errorMsg = "Unexpected character: " + std::string(1, c) +
                           " at line " + std::to_string(line) + ", column " +
                           std::to_string(start_column);
    throw utils::SQLError(errorMsg, line, start_column);
  }
}

Token Lexer::parseIdentifierOrKeyword() {
  size_t start_column = column;
  std::string value;

  while (std::isalnum(peek()) || peek() == '_') {
    value += advance();
  }

  auto upperValue = utils::toUpper(value);
  auto it = KEYWORDS.find(upperValue);
  if (it != KEYWORDS.end()) {
    return Token(it->second, value, line, start_column);
  }

  return Token(TokenType::IDENTIFIER, value, line, start_column);
}

Token Lexer::parseNumber() {
  size_t start_column = column;
  std::string value;
  bool isReal = false;

  while (std::isdigit(peek()) || peek() == '.') {
    char c = advance();
    if (c == '.') {
      if (isReal) {
        throw utils::SQLError("Invalid number format: multiple decimal points",
                              line, start_column);
      }
      isReal = true;
    }
    value += c;
  }

  return Token(isReal ? TokenType::FLOAT_LIT : TokenType::INTEGER_LIT, value,
               line, start_column);
}

Token Lexer::parseString() {
  size_t start_column = column;
  advance(); // 跳过开始的单引号
  std::string value;

  while (peek() != '\'' && peek() != '\0') {
    value += advance();
  }

  if (peek() == '\0') {
    throw utils::SQLError("Unterminated string literal", line, start_column);
  }

  advance(); // 跳过结束的单引号
  return Token(TokenType::STRING_LIT, value, line, start_column);
}