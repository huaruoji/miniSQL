#pragma once
#include "utils.hpp"
#include <map>
#include <string>
#include <vector>

class Table {
public:
  Table(std::string name, const std::vector<utils::Column> &columns);

  // Data manipulation
  void insert(const std::vector<utils::Value> &values);
  void
  update(const std::vector<std::pair<std::string, utils::Value>> &assignments,
         const std::vector<utils::Condition> &conditions);
  void deleteRows(const std::vector<utils::Condition> &conditions);

  // Queries
  std::vector<std::vector<utils::Value>>
  select(const std::vector<std::string> &columns,
         const std::vector<utils::Condition> &conditions,
         const std::vector<utils::JoinClause> &joins);

  // Table info
  const std::vector<utils::Column> &getColumns() const { return columns_; }
  const std::string &getName() const { return name_; }

private:
  std::string name_;
  std::vector<utils::Column> columns_;
  std::vector<std::vector<utils::Value>> rows_;
  std::map<std::string, size_t> columnIndexMap;

  bool matchesConditions(const std::vector<utils::Value> &row,
                         const std::vector<utils::Condition> &conditions) const;
  bool matchesCondition(const utils::Value &value, utils::ComparisonOp op,
                        const utils::Value &target) const;
  std::vector<size_t>
  getColumnIndices(const std::vector<std::string> &columnNames) const;
  void validateValues(const std::vector<utils::Value> &values) const;
};