cmake_minimum_required(VERSION 3.10)

project(converter LANGUAGES C)

add_library(converter src/converter.c)

target_include_directories(converter PUBLIC include)
