### Supporting Data Types

- INTEGER
- FLOAT
- TEXT

### Syntax of miniSQL

#### 2.1 Create Database and Use Database
- **Create Database**
  ```sql
  CREATE DATABASE database_name;
  ```

- **Use Database**
  ```sql
  USE DATABASE database_name;
  ```

#### 2.2 Create Tables
- **Create Table**
  ```sql
  CREATE TABLE table_name (
      column1_name column1_type,
      column2_name column2_type,
      column3_name column3_type,
      ...
  );
  ```

#### 2.3 Drop Tables
- **Drop Table**
  ```sql
  DROP TABLE table_name;
  ```

#### 2.4 Data Insertion
- **Insert Into**
  ```sql
  INSERT INTO table_name VALUES (value1, value2, ...);
  ```

#### 2.5 Data Query: Basics
- **Select Specific Columns**
  ```sql
  SELECT column1, column2, ... FROM table_name;
  ```

- **Select All Columns**
  ```sql
  SELECT * FROM table_name;
  ```

#### 2.6 Data Query: "Where" Clause
- **Select with Where**
  ```sql
  SELECT column1, column2 FROM table_name WHERE condition1 AND/OR condition2;
  ```

#### 2.7 Data Query: "Inner Join" Clause
- **Inner Join**
  ```sql
  SELECT table1.column1, table2.column1
  FROM table1
  INNER JOIN table2
  ON table1.X = table2.Y;
  ```

#### 2.8 Data Update
- **Update**
  ```sql
  UPDATE table_name
  SET column1 = new_value1, column2 = new_value2, ...
  WHERE condition;
  ```

#### 2.9 Data Deletion
- **Delete**
  ```sql
  DELETE FROM table_name
  WHERE condition;
  ```