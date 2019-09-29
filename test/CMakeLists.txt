cmake_minimum_required(VERSION 3.15)

project(test LANGUAGES C)

add_executable(test src/test.c)
target_link_libraries(test converter)
