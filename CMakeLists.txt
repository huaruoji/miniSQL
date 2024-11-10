cmake_minimum_required(VERSION 3.10)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/database.cpp
    src/table.cpp
)

# Add header files
set(HEADERS
    src/utils.hpp
    src/lexer.hpp
    src/parser.hpp
    src/database.hpp
    src/table.hpp
)

# Create executable
add_executable(minidb ${SOURCES} ${HEADERS})

# Add compiler warnings
if(MSVC)
    target_compile_options(minidb PRIVATE /W4)
else()
    target_compile_options(minidb PRIVATE -Wall -Wextra -Wpedantic)
endif()