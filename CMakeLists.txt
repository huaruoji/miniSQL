cmake_minimum_required(VERSION 3.10)
project(minisql)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/parser.cpp
    src/lexer.cpp
    src/database.cpp
    src/table.cpp
)

# Create executable
add_executable(minidb ${SOURCES})

# Include directories
target_include_directories(minidb PRIVATE src) 