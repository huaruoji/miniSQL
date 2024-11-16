-- Test Database Operations
CREATE DATABASE university;
USE DATABASE university;

-- Test Table Creation
CREATE TABLE students (
    id INTEGER,
    name TEXT,
    gpa FLOAT
);

CREATE TABLE courses (
    code INTEGER,
    name TEXT,
    credits FLOAT
);

CREATE TABLE enrollments (
    student_id INTEGER,
    course_code INTEGER,
    grade FLOAT
);

-- Test Basic Inserts
INSERT INTO students VALUES (1001, 'John Smith', 3.50);
INSERT INTO students VALUES (1002, 'Mary Johnson', 3.80);
INSERT INTO students VALUES (1003, 'Bob Wilson', 3.20);
INSERT INTO students VALUES (1004, 'Alice Brown', 3.90);

INSERT INTO courses VALUES (101, 'Introduction to Programming', 3.00);
INSERT INTO courses VALUES (102, 'Data Structures', 4.00);
INSERT INTO courses VALUES (103, 'Database Systems', 3.00);
INSERT INTO courses VALUES (104, 'Computer Networks', 3.00);

INSERT INTO enrollments VALUES (1001, 101, 3.50);
INSERT INTO enrollments VALUES (1001, 102, 3.70);
INSERT INTO enrollments VALUES (1002, 101, 4.00);
INSERT INTO enrollments VALUES (1002, 103, 3.80);
INSERT INTO enrollments VALUES (1003, 102, 3.00);
INSERT INTO enrollments VALUES (1004, 103, 4.00);
INSERT INTO enrollments VALUES (1004, 104, 3.90);

-- Test Basic Selects
SELECT * FROM students;
SELECT id, name FROM students;
SELECT code, name, credits FROM courses;

-- Test WHERE Clause
SELECT name, gpa FROM students WHERE gpa > 3.50;
SELECT * FROM courses WHERE credits = 4.00;
SELECT * FROM enrollments WHERE grade >= 3.80;

-- Test Complex WHERE Conditions
SELECT * FROM students WHERE gpa > 3.00 AND gpa < 3.80;
SELECT * FROM enrollments WHERE student_id = 1001 OR student_id = 1002;

-- Test Joins
SELECT students.name, courses.name, enrollments.grade
FROM students
INNER JOIN enrollments ON students.id = enrollments.student_id
INNER JOIN courses ON enrollments.course_code = courses.code;

-- Test Updates
UPDATE students SET gpa = 3.60 WHERE id = 1003;
UPDATE courses SET credits = 4.00 WHERE code = 103;
UPDATE enrollments SET grade = grade + 0.10 WHERE student_id = 1001;

-- Verify Updates
SELECT * FROM students WHERE id = 1003;
SELECT * FROM courses WHERE code = 103;
SELECT * FROM enrollments WHERE student_id = 1001;

-- Test Multiple Column Updates
UPDATE students 
SET gpa = 3.90, name = 'Robert Wilson' 
WHERE id = 1003;

-- Test Deletes
DELETE FROM enrollments WHERE grade < 3.50;
DELETE FROM students WHERE gpa < 3.00;

-- Verify Deletes
SELECT * FROM enrollments;
SELECT * FROM students;

-- Test Drop Tables
DROP TABLE enrollments;
DROP TABLE courses;
DROP TABLE students;

-- Test Creating Another Database
CREATE DATABASE school;
USE DATABASE school;

-- Verify Previous Database Tables are Gone
CREATE TABLE students (
    id INTEGER,
    name TEXT,
    gpa FLOAT
); 