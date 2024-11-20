#pragma once

#include <cassert>
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

class TableError : public std::runtime_error {
public:
  explicit TableError(const std::string &message)
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
  bool empty_statement = true;
  for (char c : current_statement) {
    if (!isspace(c))
      empty_statement = false;
  }
  if (!empty_statement) {
    throw ParseError("Unterminated statement");
  }

  return statements;
}

// Token types and value types

using Value = std::variant<std::string, int, double>;
enum class TokenType {
  EOF_TOKEN,
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
  STRING_LITERAL,
  COMMA,
  SEMICOLON,
  LEFT_PAREN,
  RIGHT_PAREN,
  EQUALS,
  GREATER_THAN,
  LESS_THAN,
  DOT,
  ASTERISK,
  PLUS,
  MINUS
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
    {"*", TokenType::ASTERISK},    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
};

const std::unordered_map<TokenType, std::string> TOKEN_STR = {
    {TokenType::CREATE, "CREATE"},
    {TokenType::DATABASE, "DATABASE"},
    {TokenType::USE, "USE"},
    {TokenType::TABLE, "TABLE"},
    {TokenType::DROP, "DROP"},
    {TokenType::INSERT, "INSERT"},
    {TokenType::INTO, "INTO"},
    {TokenType::VALUES, "VALUES"},
    {TokenType::SELECT, "SELECT"},
    {TokenType::FROM, "FROM"},
    {TokenType::WHERE, "WHERE"},
    {TokenType::UPDATE, "UPDATE"},
    {TokenType::SET, "SET"},
    {TokenType::DELETE, "DELETE"},
    {TokenType::INNER, "INNER"},
    {TokenType::JOIN, "JOIN"},
    {TokenType::ON, "ON"},
    {TokenType::AND, "AND"},
    {TokenType::OR, "OR"},
    {TokenType::INTEGER, "INTEGER"},
    {TokenType::FLOAT, "FLOAT"},
    {TokenType::TEXT, "TEXT"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"},
    {TokenType::FLOAT_LITERAL, "FLOAT_LITERAL"},
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::LEFT_PAREN, "LEFT_PAREN"},
    {TokenType::RIGHT_PAREN, "LEFT_PAREN"},
    {TokenType::EQUALS, "EQUALS"},
    {TokenType::GREATER_THAN, "GREATER_THAN"},
    {TokenType::LESS_THAN, "LESS_THAN"},
    {TokenType::DOT, "DOT"},
    {TokenType::ASTERISK, "ASTERISK"},
    {TokenType::EOF_TOKEN, "EOF_TOKEN"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
};

struct Token {
  TokenType type;
  std::string value;

  Token(TokenType t, std::string v = "") : type(t), value(v) {}
};

inline Token recognizeToken(const std::string &token) {
  if (TOKEN_MAP.find(token) != TOKEN_MAP.end()) {
    return Token(TOKEN_MAP.at(token), token);
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

const std::unordered_map<SQLStatementType, std::string> SQL_STATEMENT_TYPE_STR =
    {
        {SQLStatementType::CREATE_DATABASE, "CREATE_DATABASE"},
        {SQLStatementType::USE_DATABASE, "USE_DATABASE"},
        {SQLStatementType::CREATE_TABLE, "CREATE_TABLE"},
        {SQLStatementType::DROP_TABLE, "DROP_TABLE"},
        {SQLStatementType::INSERT, "INSERT"},
        {SQLStatementType::SELECT, "SELECT"},
        {SQLStatementType::UPDATE, "UPDATE"},
        {SQLStatementType::DELETE, "DELETE"},
        {SQLStatementType::INNER_JOIN, "INNER_JOIN"},
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
  std::string column_name_a;
  std::string column_name_b;
  TokenType operator_type;
  Value value;
};

struct ColumnDefinition {
  std::string name;
  TokenType type;
};

class FileWriter {
private:
  std::ofstream file;

public:
  explicit FileWriter() = default;

  void open(const std::string &filename) {
    file.open(filename);
    if (!file.is_open()) {
      throw FileError("Failed to open output file");
    }
  }

  void write(const std::string &data) { file << "\"" << data << "\""; }

  void write(int data) { file << data; }

  void write(double data) { file << data; }

  void write(const Value &data) {
    if (std::holds_alternative<int>(data)) {
      write(std::get<int>(data));
    } else if (std::holds_alternative<double>(data)) {
      write(std::get<double>(data));
    } else if (std::holds_alternative<std::string>(data)) {
      write(std::get<std::string>(data));
    }
  }

  void write(const std::vector<Value> &row) {
    for (const auto &value : row) {
      write(value);
      if (&value != &row.back()) {
        file << ",";
      }
    }
  }

  void write(const std::vector<std::vector<Value>> &rows) {
    if (file.tellp() > 0) {
      file << "---\n";
    }
    for (const auto &row : rows) {
      write(row);
      file << "\n";
    }
  }
};

extern FileWriter file_writer;