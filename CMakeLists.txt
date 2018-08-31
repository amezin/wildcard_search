cmake_minimum_required(VERSION 3.1)

project(wildcard-search)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} wildcard_search.cpp wildcard_search.h)
add_executable(check test.cpp slower_solutions_for_tests.cpp slower_solutions_for_tests.h)
target_link_libraries(check ${PROJECT_NAME})

enable_testing()
add_test(NAME check COMMAND check)
