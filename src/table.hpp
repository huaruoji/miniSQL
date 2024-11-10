#pragma once
#include "utils.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class Table {
public:
  Table(const std::string &name, const std::vector<utils::Column> &columns);

  // 基本操作
  void insert(const std::vector<utils::Value> &values);
  void
  update(const std::vector<std::pair<std::string, utils::Value>> &assignments,
         const std::vector<utils::Condition> &conditions);
  void deleteRows(const std::vector<utils::Condition> &conditions);

  // 查询操作
  std::vector<std::vector<utils::Value>>
  select(const std::vector<std::string> &columns,
         const std::vector<utils::Condition> &conditions = {},
         const std::vector<utils::JoinClause> &joins = {});

  // 获取表的元数据
  const std::vector<utils::Column> &getColumns() const { return columns; }
  const std::string &getName() const { return tableName; }

private:
  std::string tableName;
  std::vector<utils::Column> columns;
  std::vector<std::vector<utils::Value>> rows;
  std::unordered_map<std::string, size_t> columnIndexMap;

  // 辅助函数
  bool matchesConditions(const std::vector<utils::Value> &row,
                         const std::vector<utils::Condition> &conditions) const;
  bool matchesCondition(const utils::Value &value, utils::ComparisonOp op,
                        const utils::Value &target) const;
  std::vector<size_t>
  getColumnIndices(const std::vector<std::string> &columnNames) const;
  void validateValues(const std::vector<utils::Value> &values) const;
};