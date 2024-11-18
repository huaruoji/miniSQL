#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// Error classes

class ArgumentError : public std::runtime_error {
public:
  explicit ArgumentError(const std::string &message)
      : std::runtime_error(message) {}
};

class FileError : public std::runtime_error {
public:
  explicit FileError(const std::string &message)
      : std::runtime_error(message) {}
};

class DatabaseError : public std::runtime_error {
public:
  explicit DatabaseError(const std::string &message)
      : std::runtime_error(message) {}
};

class ParseError : public std::runtime_error {
public:
  explicit ParseError(const std::string &message)
      : std::runtime_error(message) {}
};

inline std::vector<std::string> splitStatements(std::ifstream &input_file) {
  std::vector<std::string> statements;
  std::string current_statement;
  char ch;

  // Read file character by character and build statements
  while (input_file.get(ch)) {
    current_statement += ch;
    if (ch == ';' && !current_statement.empty()) {
      statements.push_back(current_statement);
      current_statement.clear();
    }
  }

  // check if there is a statement left
  if (!current_statement.empty()) {
    throw ParseError("Unterminated statement");
  }

  return statements;
}

// Token types and value types

using Value = std::variant<std::string, int, double>;
enum class TokenType {
  CREATE,
  DATABASE,
  USE,
  TABLE,
  DROP,
  INSERT,
  INTO,
  VALUES,
  SELECT,
  FROM,
  WHERE,
  UPDATE,
  SET,
  DELETE,
  INNER,
  JOIN,
  ON,
  AND,
  OR,
  INTEGER,
  FLOAT,
  TEXT,
  IDENTIFIER,
  INTEGER_LITERAL,
  FLOAT_LITERAL,
  COMMA,
  SEMICOLON,
  LEFT_PAREN,
  RIGHT_PAREN,
  EQUALS,
  GREATER_THAN,
  LESS_THAN,
  DOT,
  ASTERISK,
  EOF_TOKEN
};

const std::unordered_map<std::string, TokenType> TOKEN_MAP = {
    {"CREATE", TokenType::CREATE}, {"DATABASE", TokenType::DATABASE},
    {"USE", TokenType::USE},       {"TABLE", TokenType::TABLE},
    {"DROP", TokenType::DROP},     {"INSERT", TokenType::INSERT},
    {"INTO", TokenType::INTO},     {"VALUES", TokenType::VALUES},
    {"SELECT", TokenType::SELECT}, {"FROM", TokenType::FROM},
    {"WHERE", TokenType::WHERE},   {"UPDATE", TokenType::UPDATE},
    {"SET", TokenType::SET},       {"DELETE", TokenType::DELETE},
    {"INNER", TokenType::INNER},   {"JOIN", TokenType::JOIN},
    {"ON", TokenType::ON},         {"AND", TokenType::AND},
    {"OR", TokenType::OR},         {"INTEGER", TokenType::INTEGER},
    {"FLOAT", TokenType::FLOAT},   {"TEXT", TokenType::TEXT},
    {",", TokenType::COMMA},       {";", TokenType::SEMICOLON},
    {"(", TokenType::LEFT_PAREN},  {")", TokenType::RIGHT_PAREN},
    {"=", TokenType::EQUALS},      {">", TokenType::GREATER_THAN},
    {"<", TokenType::LESS_THAN},   {".", TokenType::DOT},
    {"*", TokenType::ASTERISK},
};

struct Token {
  TokenType type;
  std::string value;

  Token(TokenType t, std::string v = "") : type(t), value(v) {}
};

enum class SQLStatementType {
  CREATE_DATABASE,
  USE_DATABASE,
  CREATE_TABLE,
  DROP_TABLE,
  INSERT,
  SELECT,
  UPDATE,
  DELETE,
  INNER_JOIN
};

struct WhereCondition {
  TokenType logic_operator = TokenType::EOF_TOKEN;
  std::string column_name_a;
  std::string column_name_b;
  TokenType condition_type_a;
  TokenType condition_type_b;
  Value value_a;
  Value value_b;
};
struct SetCondition {
  std::string column_name;
  Value value;
};
struct ColumnDefinition {
  std::string name;
  TokenType type;
};