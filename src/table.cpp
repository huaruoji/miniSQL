#include "table.hpp"
#include <algorithm>
#include <stdexcept>

Table::Table(std::string name, const std::vector<utils::Column> &cols)
    : name_(std::move(name)), columns_(cols) {
  // 初始化列索引映射
  for (size_t i = 0; i < columns_.size(); ++i) {
    columnIndexMap[columns_[i].name] = i;
  }
}

void Table::insert(const std::vector<utils::Value> &values) {
  // TODO: 实现插入操作
  validateValues(values);
  rows_.push_back(values);
}

void Table::update(
    const std::vector<std::pair<std::string, utils::Value>> &assignments,
    const std::vector<utils::Condition> &conditions) {
  // TODO: 实现更新操作
}

void Table::deleteRows(const std::vector<utils::Condition> &conditions) {
  // TODO: 实现删除操作
}

std::vector<std::vector<utils::Value>>
Table::select(const std::vector<std::string> &columns,
              const std::vector<utils::Condition> &conditions,
              const std::vector<utils::JoinClause> &joins) {
  // TODO: 实现查询操作
  return {};
}

bool Table::matchesConditions(
    const std::vector<utils::Value> &row,
    const std::vector<utils::Condition> &conditions) const {
  // TODO: 实现条件匹配
  return false;
}

bool Table::matchesCondition(const utils::Value &value, utils::ComparisonOp op,
                             const utils::Value &target) const {
  // TODO: 实现单个条件匹配
  return false;
}

std::vector<size_t>
Table::getColumnIndices(const std::vector<std::string> &columnNames) const {
  // TODO: 实现列索引查找
  return {};
}

void Table::validateValues(const std::vector<utils::Value> &values) const {
  // TODO: 实现值验证
}