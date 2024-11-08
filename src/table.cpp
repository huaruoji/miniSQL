#include "table.hpp"
#include "utils.hpp"
#include <algorithm>
#include <stdexcept>

Table::Table(const std::string &name, const std::vector<Column> &cols)
    : tableName(name), columns(cols) {
  // TODO: 验证列名的唯一性
  // TODO: 初始化表的基本结构
}

void Table::insert(const std::vector<Value> &values) {
  // TODO: 检查值的数量是否匹配列数
  // TODO: 检查每个值的类型是否匹配对应列的类型
  // TODO: 将值添加到表中
  // TODO: 更新所有相关的索引
}

std::vector<std::vector<Value>>
Table::select(const std::vector<std::string> &selectColumns) {
  // TODO: 验证请求的列名是否存在
  // TODO: 获取列的索引位置
  // TODO: 从表中提取相应的列数据
  // TODO: 如果有索引，考虑使用索引优化查询
  return {};
}

void Table::createIndex(const std::string &columnName) {
  // TODO: 检查列是否存在
  // TODO: 检查索引是否已经存在
  // TODO: 找到列的位置
  // TODO: 为该列创建索引结构
  // TODO: 遍历所有现有数据，填充索引
}