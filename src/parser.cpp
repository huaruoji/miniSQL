#include "parser.hpp"
#include <algorithm>
#include <sstream>


Query Parser::parse(const std::string &sql) {
  // TODO: 解析SQL语句的类型
  // TODO: 根据类型调用相应的解析函数
  return Query{};
}

Query Parser::parseCreateTable(const std::string &sql) {
  // TODO: 解析CREATE TABLE语句
  return Query{};
}

Query Parser::parseDropTable(const std::string &sql) {
  // TODO: 解析DROP TABLE语句
  return Query{};
}

Query Parser::parseInsert(const std::string &sql) {
  // TODO: 解析INSERT语句
  return Query{};
}

Query Parser::parseSelect(const std::string &sql) {
  // TODO: 解析SELECT语句
  return Query{};
}

Query Parser::parseCreateIndex(const std::string &sql) {
  // TODO: 解析CREATE INDEX语句
  return Query{};
}