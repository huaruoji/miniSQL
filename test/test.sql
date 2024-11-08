-- Database operations
CREATE DATABASE school;
USE DATABASE school;

-- Create tables
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

-- Insert test data into students
INSERT INTO students VALUES (1001, 'John Smith', 3.5);
INSERT INTO students VALUES (1002, 'Mary Johnson', 3.8);
INSERT INTO students VALUES (1003, 'Bob Wilson', 3.2);
INSERT INTO students VALUES (1004, 'Alice Brown', 3.9);

-- Insert test data into courses
INSERT INTO courses VALUES (101, 'Introduction to Programming', 3.0);
INSERT INTO courses VALUES (102, 'Data Structures', 4.0);
INSERT INTO courses VALUES (103, 'Database Systems', 3.0);
INSERT INTO courses VALUES (104, 'Computer Networks', 3.0);

-- Insert test data into enrollments
INSERT INTO enrollments VALUES (1001, 101, 3.5);
INSERT INTO enrollments VALUES (1001, 102, 3.7);
INSERT INTO enrollments VALUES (1002, 101, 4.0);
INSERT INTO enrollments VALUES (1002, 103, 3.8);
INSERT INTO enrollments VALUES (1003, 102, 3.0);
INSERT INTO enrollments VALUES (1004, 103, 4.0);
INSERT INTO enrollments VALUES (1004, 104, 3.9);

-- Basic SELECT queries
SELECT id, name, gpa FROM students;
SELECT * FROM courses;

-- WHERE clause tests
SELECT name, gpa FROM students WHERE gpa > 3.5;
SELECT code, name FROM courses WHERE credits >= 4.0;

-- JOIN operations
SELECT students.name, courses.name, enrollments.grade
FROM students
INNER JOIN enrollments ON students.id = enrollments.student_id
INNER JOIN courses ON enrollments.course_code = courses.code;

-- UPDATE operations
UPDATE students SET gpa = 3.6 WHERE id = 1003;
SELECT id, name, gpa FROM students WHERE id = 1003;

-- DELETE operations
DELETE FROM enrollments WHERE student_id = 1001 AND course_code = 101;
SELECT * FROM enrollments WHERE student_id = 1001;

-- Complex queries
SELECT students.name, AVG(enrollments.grade) as avg_grade
FROM students
INNER JOIN enrollments ON students.id = enrollments.student_id
GROUP BY students.id
HAVING avg_grade > 3.5;

-- Drop table test
DROP TABLE enrollments;
DROP TABLE courses;
DROP TABLE students; 