#pragma once
#include <queue>
#include <string>
#include <vector>


enum class TokenType {
  // 关键字
  CREATE,
  TABLE,
  DROP,
  INSERT,
  INTO,
  VALUES,
  SELECT,
  FROM,
  INDEX,
  ON,

  // 数据类型
  INTEGER,
  TEXT,
  REAL,

  // 符号
  SEMICOLON, // ;
  COMMA,     // ,
  LPAREN,    // (
  RPAREN,    // )

  // 其他
  IDENTIFIER,  // 表名、列名等
  INTEGER_LIT, // 整数字面量
  REAL_LIT,    // 浮点数字面量
  STRING_LIT,  // 字符串字面量

  END // 结束标记
};

struct Token {
  TokenType type;
  std::string value;

  Token(TokenType t, const std::string &v = "") : type(t), value(v) {}
};

class Lexer {
public:
  Lexer(const std::string &input);

  // 获取下一个token
  Token nextToken();

  // 预览下一个token但不消费
  Token peekToken();

  // 检查是否还有更多token
  bool hasMoreTokens() const;

private:
  std::string input;
  size_t position;
  std::queue<Token> tokenBuffer;

  // 辅助函数
  void skipWhitespace();
  char peek() const;
  char advance();
  bool isDigit(char c) const;
  bool isLetter(char c) const;
  bool isIdentifierChar(char c) const;

  // 解析具体类型
  Token parseIdentifier();
  Token parseNumber();
  Token parseString();

  // 关键字映射
  static bool isKeyword(const std::string &word, TokenType &type);
};