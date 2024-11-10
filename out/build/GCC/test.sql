-- Database Creation and Use
CREATE DATABASE university;
USE DATABASE university;

-- Table Creation
CREATE TABLE students (
    ID INTEGER,
    Name TEXT,
    GPA FLOAT,
    Major TEXT
);

CREATE TABLE courses (
    CourseID INTEGER,
    CourseName TEXT,
    Credits FLOAT
);

CREATE TABLE enrollments (
    StudentID INTEGER,
    CourseID INTEGER,
    Grade FLOAT
);

-- Data Insertion for students
INSERT INTO students VALUES (1001, 'Jay Chou', 3.5, 'Computer Science');
INSERT INTO students VALUES (1002, 'Taylor Swift', 3.8, 'Music');
INSERT INTO students VALUES (1003, 'Bob Dylan', 3.2, 'Literature');
INSERT INTO students VALUES (1004, 'Bruno Mars', 3.6, 'Music');
INSERT INTO students VALUES (1005, 'Alan Turing', 4.0, 'Computer Science');

-- Data Insertion for courses
INSERT INTO courses VALUES (101, 'Introduction to Programming', 3.0);
INSERT INTO courses VALUES (102, 'Music Theory', 3.0);
INSERT INTO courses VALUES (103, 'Creative Writing', 3.0);
INSERT INTO courses VALUES (104, 'Data Structures', 4.0);

-- Data Insertion for enrollments
INSERT INTO enrollments VALUES (1001, 101, 3.7);
INSERT INTO enrollments VALUES (1001, 104, 3.3);
INSERT INTO enrollments VALUES (1002, 102, 4.0);
INSERT INTO enrollments VALUES (1003, 103, 3.5);
INSERT INTO enrollments VALUES (1004, 102, 3.8);
INSERT INTO enrollments VALUES (1005, 101, 4.0);
INSERT INTO enrollments VALUES (1005, 104, 4.0);

-- Basic SELECT queries
SELECT * FROM students;
SELECT Name, GPA FROM students;

-- WHERE clause queries
SELECT Name, GPA FROM students WHERE GPA > 3.5;
SELECT Name, Major FROM students WHERE Major = 'Computer Science';
SELECT Name FROM students WHERE GPA > 3.5 AND Major = 'Computer Science';
SELECT Name FROM students WHERE Major = 'Music' OR Major = 'Literature';

-- INNER JOIN queries
SELECT students.Name, courses.CourseName, enrollments.Grade
FROM students
INNER JOIN enrollments ON students.ID = enrollments.StudentID
INNER JOIN courses ON enrollments.CourseID = courses.CourseID;

SELECT students.Name, courses.CourseName
FROM students
INNER JOIN enrollments ON students.ID = enrollments.StudentID
INNER JOIN courses ON enrollments.CourseID = courses.CourseID
WHERE enrollments.Grade = 4.0;

-- UPDATE operations
UPDATE students
SET GPA = 3.9
WHERE Name = 'Jay Chou';

UPDATE students
SET Major = 'Computer Science', GPA = GPA + 0.1
WHERE GPA > 3.5;

-- Verify updates
SELECT * FROM students;

-- DELETE operations
DELETE FROM enrollments
WHERE Grade < 3.5;

-- Verify deletions
SELECT * FROM enrollments;

-- DROP table and recreate
DROP TABLE enrollments;

CREATE TABLE enrollments (
    StudentID INTEGER,
    CourseID INTEGER,
    Grade FLOAT
);

-- Test database persistence by switching databases
USE DATABASE university;
SELECT * FROM students;