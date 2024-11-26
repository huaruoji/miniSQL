# MiniSQL - A Lightweight Database Management System

[Click here for the English version](README_en.md)

## 项目简介

本项目为个人在香港科技大学（广州）UFUG2601 C++ Programming 课程（24Fall）的 Final Project。

[在 Github 上查看此项目](https://github.com/huaruoji/miniSQL)（作业提交截止后设置为 Public Repository）。

## 特性

- [x] [Project 要求中的所有功能](requirements.md)
- [x] 基本的现代 C++ 使用
- [x] 一般的效率（数据库使用 `std::list` 和 `std::vector` 实现）。
- [x] 一般的面向对象封装。

## 使用方式

查看 [release](https://github.com/huaruoji/miniSQL/releases/tag/v1.0.5) 下载可执行文件。

编译：

```bash
mkdir build
cd build
cmake ..
make
```

使用：

```bash
./minidb test.sql output.txt
````

## 项目框架

```
minisql/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── parser.hpp
│   ├── lexer.hpp
│   ├── database.hpp
│   ├── table.hpp
│   ├── utils.hpp
│   ├── statement.hpp
└── test/
    └── test.sql
```