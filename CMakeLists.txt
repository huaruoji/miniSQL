cmake_minimum_required(VERSION 3.15)
project(minisql VERSION 0.1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 源文件
set(SOURCES
    src/main.cpp
    src/database.cpp
    src/parser.cpp
    src/lexer.cpp
    src/table.cpp
    src/utils.cpp
)

# 头文件
set(HEADERS
    src/database.hpp
    src/parser.hpp
    src/lexer.hpp
    src/table.hpp
    src/utils.hpp
)

# 可执行文件
add_executable(minisql ${SOURCES} ${HEADERS})

# 包含目录
target_include_directories(minisql PRIVATE src)

# 安装规则
install(TARGETS minisql
    RUNTIME DESTINATION bin
)

# 启用测试
enable_testing()

# 添加测试用例
add_test(NAME sql_test
         COMMAND minisql ${CMAKE_SOURCE_DIR}/test/test.sql
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}) 