#include "table.hpp"
#include "utils.hpp"
#include <algorithm>
#include <stdexcept>

Table::Table(const std::string &name, const std::vector<utils::Column> &cols)
    : tableName(name), columns(cols) {
  // TODO: 验证列名的唯一性
  // TODO: 初始化表的基本结构
  // TODO: 初始化 columnIndexMap
}

void Table::insert(const std::vector<utils::Value> &values) {
  // TODO: 检查值的数量是否匹配列数
  // TODO: 检查每个值的类型是否匹配对应列的类型
  // TODO: 将值添加到表中
}

void Table::update(
    const std::vector<std::pair<std::string, utils::Value>> &assignments,
    const std::vector<utils::Condition> &conditions) {
  // TODO: 验证列名是否存在
  // TODO: 验证值的类型是否匹配
  // TODO: 更新满足条件的行
}

void Table::deleteRows(const std::vector<utils::Condition> &conditions) {
  // TODO: 删除满足条件的行
}

std::vector<std::vector<utils::Value>>
Table::select(const std::vector<std::string> &columns,
              const std::vector<utils::Condition> &conditions,
              const std::vector<utils::JoinClause> &joins) {
  // TODO: 验证请求的列名是否存在
  // TODO: 处理 JOIN 操作
  // TODO: 应用 WHERE 条件
  // TODO: 返回结果集
  return {};
}

bool Table::matchesConditions(
    const std::vector<utils::Value> &row,
    const std::vector<utils::Condition> &conditions) const {
  // TODO: 检查行是否满足所有条件
  return false;
}

bool Table::matchesCondition(const utils::Value &value, utils::ComparisonOp op,
                             const utils::Value &target) const {
  // TODO: 实现比较操作
  return false;
}

std::vector<size_t>
Table::getColumnIndices(const std::vector<std::string> &columnNames) const {
  // TODO: 获取列名对应的索引
  return {};
}

void Table::validateValues(const std::vector<utils::Value> &values) const {
  // TODO: 验证值的数量和类型
}