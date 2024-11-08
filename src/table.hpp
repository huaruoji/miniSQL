#pragma once
#include <map>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>


enum class ColumnType { INTEGER, TEXT, REAL };

using Value = std::variant<int, std::string, double>;

struct Column {
  std::string name;
  ColumnType type;
};

class Table {
public:
  Table(const std::string &name, const std::vector<Column> &columns);

  void insert(const std::vector<Value> &values);
  std::vector<std::vector<Value>>
  select(const std::vector<std::string> &columns);
  void createIndex(const std::string &columnName);

private:
  std::string tableName;
  std::vector<Column> columns;
  std::vector<std::vector<Value>> rows;

  // 索引结构：列名 -> (值 -> 行号的multiset)
  std::map<std::string, std::map<Value, std::multiset<size_t>>> indices;
};