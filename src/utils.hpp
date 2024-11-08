#pragma once
#include "table.hpp"
#include <algorithm>
#include <string>
#include <vector>


namespace utils {

// 字符串处理
std::string trim(const std::string &str);
std::string toUpper(const std::string &str);
std::string toLower(const std::string &str);
std::vector<std::string> split(const std::string &str, char delimiter);

// 类型转换
Value stringToValue(const std::string &str, ColumnType type);
std::string valueToString(const Value &value);

// 类型检查
bool isValidType(const Value &value, ColumnType type);
ColumnType getValueType(const Value &value);

// 错误处理
class SQLError : public std::runtime_error {
public:
  explicit SQLError(const std::string &message) : std::runtime_error(message) {}
};

} // namespace utils