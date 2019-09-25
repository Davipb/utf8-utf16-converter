cmake_minimum_required(VERSION 3.15)

project(converter)

add_library(converter src/converter.c)

target_include_directories(converter PUBLIC include)
