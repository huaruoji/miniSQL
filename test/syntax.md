### 1. 创建数据库
```sql
CREATE DATABASE database_name;
```
例如：
```sql
CREATE DATABASE university;
```

### 2. 使用数据库
```sql
USE DATABASE database_name;
```
例如：
```sql
USE DATABASE university;
```

### 3. 创建表
```sql
CREATE TABLE table_name (
    column1 datatype,
    column2 datatype,
    ...
);
```
例如：
```sql
CREATE TABLE students (
    id INTEGER,
    name TEXT,
    gpa FLOAT
);
```

### 4. 插入数据
```sql
INSERT INTO table_name VALUES (value1, value2, ...);
```
例如：
```sql
INSERT INTO students VALUES (1001, 'John Smith', 3.50);
```

### 5. 查询数据
```sql
SELECT column1, column2, ... FROM table_name WHERE condition;
```
例如：
```sql
SELECT * FROM students;
SELECT id, name FROM students;
SELECT name, gpa FROM students WHERE gpa > 3.50;
```

### 6. 更新数据
```sql
UPDATE table_name SET column1 = value1, column2 = value2, ... WHERE condition;
```
例如：
```sql
UPDATE students SET gpa = 3.60 WHERE id = 1003;
UPDATE students SET gpa = gpa + 0.10, name = 'Robert Wilson' WHERE id = 1003;
```

注意 SET 后的条件最多包含一个 operator，两个 operands，另外 operands 除了字面量只可能为该 column 自己。

### 7. 删除数据
```sql
DELETE FROM table_name WHERE condition;
```
例如：
```sql
DELETE FROM enrollments WHERE grade < 3.50;
DELETE FROM students WHERE gpa < 3.00;
```

### 8. 删除表
```sql
DROP TABLE table_name;
```
例如：
```sql
DROP TABLE enrollments;
DROP TABLE courses;
DROP TABLE students;
```

### 9. 连接查询
```sql
SELECT table1.column1, table2.column2
FROM table1
INNER JOIN table2 ON table1.column = table2.column;
```
例如：
```sql
SELECT students.name, enrollments.grade
FROM students
INNER JOIN enrollments ON students.id = enrollments.student_id;
```

### 10. 创建新的数据库和表
```sql
CREATE DATABASE new_database_name;
USE DATABASE new_database_name;

CREATE TABLE new_table_name (
    column1 datatype,
    column2 datatype,
    ...
);
```
例如：
```sql
CREATE DATABASE school;
USE DATABASE school;

CREATE TABLE students (
    id INTEGER,
    name TEXT,
    gpa FLOAT
);
```