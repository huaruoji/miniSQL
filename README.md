# MiniSQL - A Lightweight Database Management System

A course project for UFUG2601 C++ Programming at HKUST(GZ), implementing a simplified SQL database management system.

## Academic Integrity Declaration

This project is developed in accordance with the HKUST(GZ) Academic Honor Code. All work is original and completed independently.

## Overview

MiniDB is a lightweight database management system that implements core SQL functionality in a simplified manner. It supports basic database operations including:
- Database creation and selection
- Table management
- Data manipulation
- Basic and complex queries
- Data persistence

## Features

### Database Operations
- Create new databases
- Switch between databases
- Automatic data persistence

### Table Operations
- Create tables with multiple data types (INTEGER, FLOAT, TEXT)
- Drop existing tables
- Insert records
- Update records
- Delete records

### Query Capabilities
- Basic SELECT queries
- WHERE clause filtering
- INNER JOIN operations
- Multiple condition support (AND/OR)
- Column selection
- Full table selection

### Data Types Support
- INTEGER
- FLOAT
- TEXT

## Installation

### Prerequisites
- C++ compiler with C++17 support
- CMake (version 3.15 or higher)

### Build Instructions
```bash
cmake .
make
```

## Usage

### Command Line Interface

The program accepts two command-line arguments:
```bash
./minidb input.sql output.txt
```

### Supported SQL Commands

#### Database Management
```sql
CREATE DATABASE database_name;
USE DATABASE database_name;
```

#### Table Operations
```sql
CREATE TABLE table_name (
    column1_name column1_type,
    column2_name column2_type,
    ...
);

DROP TABLE table_name;
```

#### Data Manipulation
```sql
INSERT INTO table_name VALUES (value1, value2, ...);
UPDATE table_name SET column1 = value1 WHERE condition;
DELETE FROM table_name WHERE condition;
```

#### Queries
```sql
SELECT column1, column2 FROM table_name;
SELECT * FROM table_name WHERE condition1 AND condition2;
SELECT table1.column1, table2.column1 
FROM table1 
INNER JOIN table2 
ON table1.column = table2.column;
```

### Output Format

Query results are formatted in CSV format:
- Fields are comma-separated
- Text fields are enclosed in double quotes
- Numeric fields are printed directly
- Float values are formatted to two decimal places
- Multiple query results are separated by "---"

Example output:
```csv
ID,Name,GPA
1000,"Jay Chou",3.00
1001,"Taylor Swift",3.20
1002,"Bob Dylan",3.50
```

### Error Handling

The system provides:
- Syntax error detection
- Line number reporting for errors
- Input validation
- Proper error messages

## Examples

### Basic Database Operations
```sql
CREATE DATABASE db_university;
USE DATABASE db_university;

CREATE TABLE student (
    ID INTEGER,
    Name TEXT,
    GPA FLOAT
);

INSERT INTO student VALUES (1000, 'Jay Chou', 3.0);
SELECT ID, Name, GPA FROM student WHERE GPA > 3.0;
```

### Complex Queries
```sql
SELECT student.Name, course_enrollment.Course
FROM student
INNER JOIN course_enrollment 
ON student.StudentID = course_enrollment.StudentID
WHERE student.GPA > 3.0;
```

### Comments Support

```sql
-- This is a comment
```

## Limitations

- Supports only basic SQL operations
- Limited to INTEGER, FLOAT, and TEXT data types
- Simple WHERE clause conditions
- Basic INNER JOIN support only

## Structure

- `src/`: Source code
- `include/`: Header files
- `test/`: Test cases
- `CMakeLists.txt`: CMake configuration
- `CMakePresets.json`: CMake presets

## 底层实现

使用 `std::multiset` 实现索引。

```
minisql/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── parser.hpp
│   ├── parser.cpp
│   ├── lexer.hpp
│   ├── lexer.cpp
│   ├── database.hpp
│   ├── database.cpp
│   ├── table.hpp
│   ├── table.cpp
│   └── utils.hpp
└── test/
    └── test.sql
```