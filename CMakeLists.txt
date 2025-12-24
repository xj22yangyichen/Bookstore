cmake_minimum_required(VERSION 3.28)
project(BookStore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_executable(code
        src/main.cpp
        src/book.cpp
        src/bookstore.cpp
        src/log.cpp
        src/parser.cpp
)