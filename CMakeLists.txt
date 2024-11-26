cmake_minimum_required(VERSION 3.10)
project(minisql)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(minidb ${SOURCES})

# Include directories
target_include_directories(minidb PRIVATE src)

# Define debug macro for Debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(minidb PRIVATE DEBUG)
endif()