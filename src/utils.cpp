#include "utils.hpp"
#include <cctype>
#include <sstream>


namespace utils {

std::string trim(const std::string &str) {
  auto start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";

  auto end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}

std::string toUpper(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::toupper);
  return result;
}

std::string toLower(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

std::vector<std::string> split(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, delimiter)) {
    if (!token.empty()) {
      tokens.push_back(trim(token));
    }
  }

  return tokens;
}

Value stringToValue(const std::string &str, ColumnType type) {
  try {
    switch (type) {
    case ColumnType::INTEGER:
      return std::stoi(str);
    case ColumnType::REAL:
      return std::stod(str);
    case ColumnType::TEXT:
      return str;
    default:
      throw SQLError("Unknown column type");
    }
  } catch (const std::exception &e) {
    throw SQLError("Failed to convert value: " + str);
  }
}

std::string valueToString(const Value &value) {
  return std::visit(
      [](const auto &v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
          return v;
        } else {
          return std::to_string(v);
        }
      },
      value);
}

bool isValidType(const Value &value, ColumnType type) {
  switch (type) {
  case ColumnType::INTEGER:
    return std::holds_alternative<int>(value);
  case ColumnType::REAL:
    return std::holds_alternative<double>(value);
  case ColumnType::TEXT:
    return std::holds_alternative<std::string>(value);
  default:
    return false;
  }
}

ColumnType getValueType(const Value &value) {
  if (std::holds_alternative<int>(value))
    return ColumnType::INTEGER;
  if (std::holds_alternative<double>(value))
    return ColumnType::REAL;
  if (std::holds_alternative<std::string>(value))
    return ColumnType::TEXT;
  throw SQLError("Unknown value type");
}

} // namespace utils