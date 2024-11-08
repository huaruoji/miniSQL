#include "database.hpp"
#include <stdexcept>

void Database::createTable(const std::string &name,
                           const std::vector<Column> &columns) {
  // TODO: 检查表是否已存在
  // TODO: 创建新表
}

void Database::dropTable(const std::string &name) {
  // TODO: 检查表是否存在
  // TODO: 删除表
}

void Database::insert(const std::string &tableName,
                      const std::vector<Value> &values) {
  // TODO: 检查表是否存在
  // TODO: 验证值的数量和类型
  // TODO: 插入数据
}

std::vector<std::vector<Value>>
Database::select(const std::string &tableName,
                 const std::vector<std::string> &columns) {
  // TODO: 检查表是否存在
  // TODO: 验证列名
  // TODO: 执行查询并返回结果
  return {};
}

void Database::createIndex(const std::string &tableName,
                           const std::string &columnName) {
  // TODO: 检查表是否存在
  // TODO: 检查列是否存在
  // TODO: 创建索引
}