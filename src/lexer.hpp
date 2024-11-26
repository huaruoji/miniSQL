#pragma once
#include "utils.hpp"
#include <algorithm>
#include <deque>
#include <string>

class Lexer {
public:
  explicit Lexer(const std::string &input, int start_line = 1)
      : input(input), current_token(Token(TokenType::EOF_TOKEN)),
        current_line(start_line) {
    std::string current_string;
    bool parsing_str_literal = false;
    bool parsing_nagetive_number = false;
    for (size_t i = 0; i < input.length(); ++i) {
      char c = input[i];
      auto createToken = [&]() {
        // debug(parsing_nagetive_number);
        if (!current_string.empty()) {
          tokens.push_back(
              Token(recognizeToken(current_string).type,
                    (parsing_nagetive_number ? "-" : "") + current_string,
                    current_line));
        }
        current_string.clear();
        parsing_nagetive_number = false;
      };

      if (c == '\n') {
        createToken();
        current_line++;
        continue;
      }

      if (c == '\'') {
        if (parsing_str_literal) {
          tokens.push_back(
              Token(TokenType::STRING_LITERAL, current_string, current_line));
          current_string.clear();
          parsing_str_literal = false;
        } else
          parsing_str_literal = true;
      } else if (c == '(' || c == ')' || c == ',' || c == ';' || c == '>' ||
                 c == '<' || c == '*' || c == '+' ||
                 (c == '.' && !std::all_of(current_string.begin(),
                                           current_string.end(), ::isdigit))) {
        createToken();
        tokens.push_back(Token(recognizeToken(std::string(1, c)).type,
                               std::string(1, c), current_line));
      } else if (c == '!' && i + 1 < input.length() && input[i + 1] == '=') {
        createToken();
        tokens.push_back(Token(TokenType::INEQUALS, "!=", current_line));
        ++i;
      } else if (c == '-') {
        // debug(isdigit(input[i + 1]));
        if (i + 1 < input.length() && std::isdigit(input[i + 1])) {
          parsing_nagetive_number = true;
        } else {
          createToken();
          tokens.push_back(Token(recognizeToken(std::string(1, c)).type,
                                 std::string(1, c), current_line));
        }
      } else if (std::isspace(c) && !parsing_str_literal) {
        createToken();
      } else {
        current_string += c;
      }
    }

    if (!current_string.empty()) {
      tokens.push_back(Token(recognizeToken(current_string).type,
                             current_string, current_line));
    }

#ifdef DEBUG
    for (const auto &token : tokens) {
      std::cout << token.value << ' ';
    }
    std::cout << std::endl;
#endif
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