#pragma once
#include "parser.hpp"
#include <string>
#include <vector>


class Table {
public:
  Table(const std::string &name, const std::vector<ColumnDefinition> &columns);

  void insert(const std::vector<std::string> &values);
  std::vector<std::vector<std::string>> select(const SelectStatement &stmt);
  void update(const UpdateStatement &stmt);
  void deleteRows(const DeleteStatement &stmt);

  void saveToFile() const;
  void loadFromFile();

private:
  std::string name;
  std::vector<ColumnDefinition> columns;
  std::vector<std::vector<std::string>> rows;
};