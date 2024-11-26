#pragma once
#include "utils.hpp"
#include <deque>
#include <string>
#include <algorithm>

class Lexer {
public:
  explicit Lexer(const std::string &input)
      : input(input), current_token(Token(TokenType::EOF_TOKEN)), current_line(1) {
    std::string current_string;
    bool parsing_str_literal = false;
    for (char c : input) {
      auto createToken = [&]() {
        if (!current_string.empty())
          tokens.push_back(Token(recognizeToken(current_string).type, current_string, current_line));
        current_string.clear();
      };

      if (c == '\n') {
        createToken();  // Create token before incrementing line
        current_line++;
        continue;
      }

      if (c == '\'') {
        if (parsing_str_literal) {
          tokens.push_back(Token(TokenType::STRING_LITERAL, current_string, current_line));
          current_string.clear();
          parsing_str_literal = false;
        } else
          parsing_str_literal = true;
      } else if (c == '(' || c == ')' || c == ',' || c == ';' || c == '=' ||
                 c == '>' || c == '<' || c == '*' || c == '+' || c == '-' ||
                 (c == '.' && !std::all_of(current_string.begin(),
                                           current_string.end(), ::isdigit))) {
        createToken();
        tokens.push_back(Token(recognizeToken(std::string(1, c)).type, std::string(1, c), current_line));
      } else if (std::isspace(c) && !parsing_str_literal) {
        createToken();
      } else {
        current_string += c;
      }
    }

    if (!current_string.empty()) {
      tokens.push_back(Token(recognizeToken(current_string).type, current_string, current_line));
    }
  }

  Token getNextToken() {
    advance();
    return current_token;
  }

  bool exist(TokenType type) {
    for (const auto &token : tokens) {
      if (token.type == type) {
        return true;
      }
    }
    return false;
  }

private:
  std::string input;
  std::deque<Token> tokens;
  Token current_token;
  int current_line;

  void advance() {
    if (tokens.empty()) {
      current_token = Token(TokenType::EOF_TOKEN);
      return;
    }
    current_token = tokens.front();
    tokens.pop_front();
  }
};