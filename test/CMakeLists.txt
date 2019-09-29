cmake_minimum_required(VERSION 3.10)

project(tester LANGUAGES C)

add_executable(tester src/test.c)
target_link_libraries(tester converter)

IF(UNIX)
    target_link_libraries(tester m) # Link math library
ENDIF()
