# MiniSQL - A Lightweight Database Management System

[Click here for the Chinese version](README.md)

## Introduction

This project is an individual Final Project for the UFUG2601 C++ Programming course (24Fall) at the Hong Kong University of Science and Technology (Guangzhou).

[View this project on Github](https://github.com/huaruoji/miniSQL) (Set to Public Repository after assignment submission deadline).

## Project Requirements

[View the original project requirements here](requirements.md).

The aim is to implement basic SQL operations using C++:

- CREATE DATABASE
- USE DATABASE
- CREATE TABLE
- DROP TABLE
- INSERT INTO
- SELECT
- UPDATE
- DELETE
- INNER JOIN

Supports basic data types: INTEGER, FLOAT, TEXT.

## Self-evaluation

- [x] [All functionalities in the project requirements](requirements.md)
- [x] Basic usage of modern C++
- [x] Object-oriented encapsulation
- [x] Implementation of complex nested SET or WHERE clauses using recursive descent parsing for expression analysis

## Usage

Check [release](https://github.com/huaruoji/miniSQL/releases/) to download the executable file.

Compilation:

```bash
mkdir build
cd build
cmake ..
make
```

Usage:

```bash
./minidb test.sql output.txt
```

## Project Structure

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