# MiniSQL - A Lightweight Database Management System

A course project for UFUG2601 C++ Programming at HKUST(GZ), implementing a simplified SQL database management system.

## Academic Integrity Declaration

This project is developed in accordance with the HKUST(GZ) Academic Honor Code. All work is original and completed independently.

## Supported Operations

### 1. Database Management
#### Create Database
```sql
CREATE DATABASE database_name;
```
- Creates a new database
- All table content should be stored to disk before program exit

#### Use Database
```sql
USE DATABASE database_name;
```
- Switches to specified database for subsequent operations
- Must be executed before any table operations
- Should load previous database content when used

### 2. Table Operations
#### Create Table
```sql
CREATE TABLE table_name (
    column1_name column1_type,
    column2_name column2_type,
    ...
);
```
Supported data types:
- INTEGER
- FLOAT
- TEXT

#### Drop Table
```sql
DROP TABLE table_name;
```
- Permanently removes the specified table and all its data

### 3. Data Operations
#### Insert Data
```sql
INSERT INTO table_name VALUES (value1, value2, ...);
```
- Values must match the column types defined in table creation
- Text values should be enclosed in single quotes

#### Update Data
```sql
UPDATE table_name
SET column1 = new_value1, column2 = new_value2, column3 = new_value3
WHERE condition;
```
- Multiple column updates are separated by commas in the SET clause
- WHERE clause is optional
- Without WHERE, all rows will be updated
- Can perform arithmetic operations in SET clause (e.g., `SET GPA = GPA + 0.1`)
- Example:
  ```sql
  UPDATE student
  SET GPA = 3.6, Major = 'Data Science'
  WHERE Name = 'Jay Chou';
  ```

#### Delete Data
```sql
DELETE FROM table_name
WHERE condition;
```
- WHERE clause is optional
- Without WHERE, all rows will be deleted
- Example:
  ```sql
  DELETE FROM student
  WHERE GPA < 3.0;
  ```

### 4. Query Operations
#### Basic Select
```sql
SELECT column1, column2, ... FROM table_name;
```
Special cases:
- `SELECT * FROM table_name;` selects all columns
- Any order of rows is acceptable in results

#### Select with WHERE Clause
```sql
SELECT column1, column2 FROM table_name WHERE condition1 AND/OR condition2;
```
Supported conditions:
- Comparisons: `>`, `<`, `=`
- Logical operators: `AND`, `OR`

#### Inner Join
```sql
SELECT table1.column1, table2.column1
FROM table1
INNER JOIN table2
ON table1.column_x = table2.column_y;
```
- Joins two tables based on matching column values
- Must use fully qualified column names (table_name.column_name)

## Output Format Requirements

### CSV Format Rules
1. Fields are separated by commas
2. First row contains column names
3. Text fields:
   - Enclosed in double quotes (`""`)
   - No text fields will contain double quotes
4. Numeric fields:
   - Integers: printed as-is
   - Floats: exactly two decimal places, rounded if necessary

### Multiple Query Results
- Results from different queries are separated by `---`
- Each result starts with a header row

Example output:
```
ID,Name,GPA
1000,"Jay Chou",3.00
1001,"Taylor Swift",3.20
---
ID,Name,GPA
1001,"Taylor Swift",3.20
```

## Error Handling
- Program should check miniSQL syntax
- Report line number where errors occur
- Handle database loading errors appropriately

## 实现思路

- 使用 lexer 分析词法
  - 处理注释、空格。
  - 在遇到分号时视为完整的查询语句。
  - 遇到其他字符则报错。
- 使用 parser 分析语法。
  - 提供报错支持。
- 使用 database 管理数据库
  - 使用 `unordered_map<std::string, std::unique_ptr<Table>>` 管理表。`unique_ptr` 可以自动管理表的生命周期。

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
│   ├── utils.hpp
└── test/
    └── test.sql
```