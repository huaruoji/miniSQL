#pragma once
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace utils {

// 字符串处理
inline std::string toUpper(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::toupper);
  return result;
}

inline std::string toLower(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
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
enum class ColumnType { INTEGER, TEXT, FLOAT };
using Value = std::variant<int, std::string, double>;

// 列定义
struct Column {
  std::string name;
  ColumnType type;
};

// 比较操作符
enum class ComparisonOp { EQUALS, GT, GTE, LT, LTE };

// 列引用
struct ColumnRef {
  std::string table; // 可能为空
  std::string column;
};

// 条件
struct Condition {
  ColumnRef column;
  ComparisonOp op;
  Value value;
};

// 连接子句
struct JoinClause {
  std::string table;
  ColumnRef leftColumn;
  ColumnRef rightColumn;
};

} // namespace utils