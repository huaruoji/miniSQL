#pragma once

// Standard library includes
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

//-----------------------------------------------------------------------------
// Debug utilities
//-----------------------------------------------------------------------------

void debug_out() { std::cerr << '\n'; }

template <typename Head, typename... Tail> void debug_out(Head H, Tail... T) {
  std::cerr << ' ' << H;
  debug_out(T...);
}

#ifdef DEBUG
#define debug(...)                                                             \
  std::cerr << '[' << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#else
#define debug(...)
#endif

//-----------------------------------------------------------------------------
// Custom exception classes
//-----------------------------------------------------------------------------

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

class SQLError : public std::runtime_error {
public:
  explicit SQLError(const std::string &message, int line = 0)
      : std::runtime_error(message), line_number(line) {}
  int getLineNumber() const { return line_number; }

protected:
  int line_number;
};

class ParseError : public SQLError {
public:
  explicit ParseError(const std::string &message, int line = 0)
      : SQLError("Parse error at line " + std::to_string(line) + ": " + message,
                 line) {}
};

class DatabaseError : public SQLError {
public:
  explicit DatabaseError(const std::string &message, int line = 0)
      : SQLError("Database error at line " + std::to_string(line) + ": " +
                     message,
                 line) {}
};

class TableError : public SQLError {
public:
  explicit TableError(const std::string &message, int line = 0)
      : SQLError("Table error at line " + std::to_string(line) + ": " + message,
                 line) {}
};

//-----------------------------------------------------------------------------
// SQL types and tokens
//-----------------------------------------------------------------------------

// Value type for storing SQL data (can be string, int, or double)
using Value = std::variant<std::string, int, double>;

// All possible SQL token types
enum class TokenType {
  // Keywords
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

  // Data types
  INTEGER,
  FLOAT,
  TEXT,

  // Literals and identifiers
  IDENTIFIER,
  INTEGER_LITERAL,
  FLOAT_LITERAL,
  STRING_LITERAL,

  // Operators and punctuation
  COMMA,
  SEMICOLON,
  LEFT_PAREN,
  RIGHT_PAREN,
  EQUALS,
  INEQUALS,
  GREATER_THAN,
  LESS_THAN,
  DOT,
  ASTERISK,
  PLUS,
  MINUS,

  // Special tokens
  EOF_TOKEN
};

// Mapping from string to token type (for lexical analysis)
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
    {"-", TokenType::MINUS},       {"!=", TokenType::INEQUALS}};

// Mapping from token type to string (for error messages and serialization)
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
    {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::EQUALS, "EQUALS"},
    {TokenType::INEQUALS, "INEQUALS"},
    {TokenType::GREATER_THAN, "GREATER_THAN"},
    {TokenType::LESS_THAN, "LESS_THAN"},
    {TokenType::DOT, "DOT"},
    {TokenType::ASTERISK, "ASTERISK"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::EOF_TOKEN, "EOF_TOKEN"}};

//-----------------------------------------------------------------------------
// SQL statement types and structures
//-----------------------------------------------------------------------------

// Token structure for lexical analysis
struct Token {
  TokenType type;
  std::string value;
  int line_number;

  Token(TokenType t = TokenType::EOF_TOKEN, std::string v = "", int line = 1)
      : type(t), value(v), line_number(line) {}
};

// SQL statement types
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

// Structure for WHERE conditions in SQL statements
struct WhereCondition {
  TokenType logic_operator = TokenType::EOF_TOKEN;
  std::string column_name_a;
  std::string column_name_b;
  TokenType condition_type_a;
  TokenType condition_type_b;
  Token value_a;
  Token value_b;
};

// Structure for SET conditions in UPDATE statements
struct SetCondition {
  std::string column_name_a;
  std::string column_name_b;
  TokenType operator_type;
  Token value;
};

// Structure for column definitions in CREATE TABLE statements
struct ColumnDefinition {
  std::string name;
  TokenType type;
};

// Base class for SQL statements
struct Statement {
  std::string content;
  int start_line;
  Statement(const std::string &content, int line)
      : content(content), start_line(line) {}
};

//-----------------------------------------------------------------------------
// Output handling
//-----------------------------------------------------------------------------

// Class for handling output to files
class OutputWriter {
private:
  std::ofstream file;

public:
  explicit OutputWriter() = default;

  void open(const std::string &filename) {
    file.open(filename);
    if (!file.is_open()) {
      throw FileError("Failed to open output file");
    }
  }

  void writeColumnNames(const std::vector<Value> &row) {
    for (const auto &value : row) {
      file << std::get<std::string>(value);
      if (&value != &row.back()) {
        file << ",";
      }
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
      if (&row != &rows.front()) {
        write(row);
      } else {
        writeColumnNames(row);
      }
      file << "\n";
    }
  }
};

extern OutputWriter file_writer;

//-----------------------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------------------

// Convert a token to its corresponding value
Value convertTokenToValue(const Token &token) {
  switch (token.type) {
  case TokenType::INTEGER_LITERAL:
    return std::stoi(token.value);
  case TokenType::FLOAT_LITERAL:
    return std::stod(token.value);
  case TokenType::STRING_LITERAL:
  case TokenType::IDENTIFIER:  // Allow identifiers to be converted to string values
    return token.value;
  default:
    throw ParseError("Invalid token type for value conversion");
  }
}

// Split input into individual SQL statements
std::vector<Statement> splitStatements(std::ifstream &input_file) {
  std::vector<Statement> statements;
  std::string current_statement;
  std::string line;
  int line_number = 0;
  int start_line = 1;

  while (std::getline(input_file, line)) {
    line_number++;

    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Remove trailing whitespace
    size_t end = line.find_last_not_of(" \t\r\n");
    if (end != std::string::npos) {
      line = line.substr(0, end + 1);
    }

    current_statement += line;

    // Process all semicolons in the line
    size_t pos = 0;
    size_t semicolon_pos;
    std::string remaining = current_statement;
    
    while ((semicolon_pos = remaining.find(';', pos)) != std::string::npos) {
      // Extract the statement up to the semicolon
      std::string statement = remaining.substr(0, semicolon_pos + 1);
      statements.emplace_back(statement, start_line);
      
      // Move past the semicolon for next search
      remaining = remaining.substr(semicolon_pos + 1);
      // Trim leading whitespace from remaining
      size_t first = remaining.find_first_not_of(" \t\r\n");
      if (first != std::string::npos) {
        remaining = remaining.substr(first);
      } else {
        remaining.clear();
      }
    }
    
    if (remaining.empty()) {
      current_statement.clear();
      start_line = line_number + 1;
    } else {
      current_statement = remaining + ' ';
    }
  }

  if (!current_statement.empty()) {
    throw ParseError("Missing semicolon at end of statement", line_number);
  }

  return statements;
}

// Token recognition
inline Token recognizeToken(std::string token) {
  // Check for keywords first
  if (TOKEN_MAP.find(token) != TOKEN_MAP.end()) {
    return Token(TOKEN_MAP.at(token), token);
  }

  // Check for integer literal
  bool is_integer = true;
  for (size_t i = 0; i < token.length(); i++) {
    char c = token[i];
    if (i == 0 && c == '-')
      continue;
    if (!std::isdigit(c)) {
      is_integer = false;
      break;
    }
  }
  if (is_integer) {
    return Token(TokenType::INTEGER_LITERAL, token);
  }

  // Check for float literal
  bool is_float = true;
  bool has_dot = false;
  for (size_t i = 0; i < token.length(); i++) {
    char c = token[i];
    if (i == 0 && c == '-')
      continue;
    if (c == '.' && !has_dot) {
      has_dot = true;
      continue;
    }
    if (!std::isdigit(c)) {
      is_float = false;
      break;
    }
  }
  if (is_float && has_dot) {
    return Token(TokenType::FLOAT_LITERAL, token);
  }

  // If not a keyword or number, it's an identifier
  return Token(TokenType::IDENTIFIER, token);
}

// Stream output operator for TokenType
inline std::ostream &operator<<(std::ostream &os, const TokenType &type) {
  auto it = TOKEN_STR.find(type);
  if (it != TOKEN_STR.end()) {
    os << it->second;
  } else {
    os << "UNKNOWN";
  }
  return os;
}