#include "lexer.hpp"
#include <algorithm>

Lexer::Lexer(const std::string &input)
    : input(input), current_token(Token(TokenType::EOF_TOKEN)) {
  std::string current_string;
  bool parsing_str_literal = false;
  for (char c : input) {
    auto createToken = [&]() {
      if (!current_string.empty())
        tokens.push_back(recognizeToken(current_string));
      current_string.clear();
    };
    if (c == '\'') {
      if (parsing_str_literal) {
        tokens.push_back(Token(TokenType::STRING_LITERAL, current_string));
        current_string.clear();
        parsing_str_literal = false;
      } else
        parsing_str_literal = true;
    } else if (c == '(' || c == ')' || c == ',' || c == ';' || c == '=' ||
               c == '>' || c == '<' || c == '*' || c == '+' || c == '-' ||
               (c == '.' && !std::all_of(current_string.begin(),
                                         current_string.end(), ::isdigit))) {
      createToken();
      tokens.push_back(recognizeToken(std::string(1, c)));
    } else if (std::isspace(c) && !parsing_str_literal) {
      createToken();
    } else {
      current_string += c;
    }
  }

  if (!current_string.empty()) {
    tokens.push_back(recognizeToken(current_string));
  }
  for (const auto &token : tokens) {
    assert(!token.value.empty());
    std::cerr << token.value << ' ';
  }
  std::cerr << std::endl;
  for (const auto &token : tokens) {
    auto it = TOKEN_STR.find(token.type);
    if (it != TOKEN_STR.end()) {
      std::cerr << it->second << ' ';
    } else {
      std::cerr << "UNKNOWN_TOKEN_TYPE" << ' ';
    }
  }
  std::cerr << std::endl;
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

bool Lexer::exist(TokenType type) {
  for (const auto &token : tokens) {
    if (token.type == type) {
      return true;
    }
  }
  return false;
}