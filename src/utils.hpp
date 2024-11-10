#pragma once
#include <algorithm>
#include <exception>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace utils {

// Token 相关定义
enum class TokenType {
  KEYWORD,
  IDENTIFIER,
  NUMBER,
  STRING,
  OPERATOR,
  PUNCTUATION,
  END_OF_STATEMENT,
  END_OF_FILE
};

struct Token {
  TokenType type;
  std::string value;
  size_t line;
  size_t column;

  Token(TokenType t, std::string v, size_t l = 0, size_t c = 0)
      : type(t), value(std::move(v)), line(l), column(c) {}
};

// 字符串处理
inline std::string toUpper(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::toupper);
  return result;
}

// 错误处理
class SQLError : public std::runtime_error {
  size_t line_number;
  size_t column_number;

public:
  explicit SQLError(const std::string &message, size_t line = 0,
                    size_t column = 0)
      : std::runtime_error(message), line_number(line), column_number(column) {}

  size_t getLine() const { return line_number; }
  size_t getColumn() const { return column_number; }
};

// 基本类型定义
enum class DataType { INTEGER, TEXT, FLOAT };
using Value = std::variant<int, double, std::string>;

// 列定义
struct Column {
  std::string name;
  DataType type;

  Column(std::string n, DataType t) : name(std::move(n)), type(t) {}
};

// 比较操作符
enum class ComparisonOp { EQUALS, GT, GTE, LT, LTE };

// 列引用
struct ColumnRef {
  std::string table;
  std::string column;

  ColumnRef(std::string t = "", std::string c = "")
      : table(std::move(t)), column(std::move(c)) {}
};

// 条件
struct Condition {
  ColumnRef column;
  ComparisonOp op;
  Value value;

  Condition(ColumnRef c, ComparisonOp o, Value v)
      : column(std::move(c)), op(o), value(std::move(v)) {}
};

// 连接子句
struct JoinClause {
  std::string table;
  ColumnRef leftColumn;
  ColumnRef rightColumn;

  JoinClause(std::string t, ColumnRef left, ColumnRef right)
      : table(std::move(t)), leftColumn(std::move(left)),
        rightColumn(std::move(right)) {}
};

} // namespace utils