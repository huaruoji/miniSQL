#include "lexer.hpp"
Lexer::Lexer(const std::string &input)
    : input(input), current_token(Token(TokenType::EOF_TOKEN)) {
  std::string current_string;
  for (char c : input) {
    auto createToken = [&](const std::string &str) {
      if (!str.empty())
        tokens.push_back(recognizeToken(str));
    };
    if (c == '(' || c == ')' || c == ',' || c == ';' || c == '=' || c == '>' ||
        c == '<' || c == '*') {
      createToken(current_string);
      current_string.clear();
      tokens.push_back(recognizeToken(std::string(1, c)));
    } else if (std::isspace(c) || c == '\n') {
      createToken(current_string);
      current_string.clear();
    } else {
      current_string += c;
    }
  }

  if (!current_string.empty()) {
    tokens.push_back(recognizeToken(current_string));
  }
  for (const auto &token : tokens) {
    std::cerr << token.value << std::endl;
  }
}

Token Lexer::recognizeToken(const std::string &token) {
  if (TOKEN_MAP.find(token) != TOKEN_MAP.end()) {
    return Token(TOKEN_MAP.at(token));
  }

  // Check if token is an integer literal
  bool is_integer = true;
  for (char c : token) {
    if (!std::isdigit(c)) {
      is_integer = false;
      break;
    }
  }
  if (is_integer) {
    return Token(TokenType::INTEGER_LITERAL, token);
  }

  // Check if token is a float literal
  bool is_float = true;
  int decimal_count = 0;
  for (size_t i = 0; i < token.length(); i++) {
    char c = token[i];
    if (c == '.') {
      ++decimal_count;
    } else if (!std::isdigit(c)) {
      is_float = false;
      break;
    }
  }
  if (is_float && decimal_count == 1) {
    return Token(TokenType::FLOAT_LITERAL, token);
  }

  return Token(TokenType::IDENTIFIER, token);
}

void Lexer::advance() {
  if (tokens.empty()) {
    current_token = Token(TokenType::EOF_TOKEN);
    return;
  }
  current_token = tokens.front();
  tokens.pop_front();
}

Token Lexer::getNextToken() {
  advance();
  return current_token;
}