cmake_minimum_required(VERSION 3.10)

project(tester LANGUAGES C)

add_executable(tester src/test.c)
target_link_libraries(tester converter)

IF(UNIX)
    target_link_libraries(tester m) # Link math library
ENDIF()

# Tests

set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test-cases)

## Two-Way

add_test(
    NAME utf8.chinese
    COMMAND tester utf8 two-way/chinese.utf8.txt two-way/chinese.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf16.chinese
    COMMAND tester utf16 two-way/chinese.utf16.txt two-way/chinese.utf8.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf8.emoji
    COMMAND tester utf8 two-way/emoji.utf8.txt two-way/emoji.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf16.emoji
    COMMAND tester utf16 two-way/emoji.utf16.txt two-way/emoji.utf8.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf8.lorem_ipsum
    COMMAND tester utf8 two-way/lorem_ipsum.utf8.txt two-way/lorem_ipsum.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf16.lorem_ipsum
    COMMAND tester utf16 two-way/lorem_ipsum.utf16.txt two-way/lorem_ipsum.utf8.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf8.bible
    COMMAND tester utf8 two-way/bible.utf8.txt two-way/bible.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf16.bible
    COMMAND tester utf16 two-way/bible.utf16.txt two-way/bible.utf8.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf8.all
    COMMAND tester utf8 two-way/all.utf8.txt two-way/all.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf16.all
    COMMAND tester utf16 two-way/all.utf16.txt two-way/all.utf8.txt
    WORKING_DIRECTORY ${TEST_DIR}
)


## One-Way: UTF-8 -> UTF-16

add_test(
    NAME utf8.overlong
    COMMAND tester utf8 utf8-to-utf16/overlong.utf8.txt utf8-to-utf16/invalid.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf8.rogue
    COMMAND tester utf8 utf8-to-utf16/rogue.utf8.txt utf8-to-utf16/invalid.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf8.surrogate
    COMMAND tester utf8 utf8-to-utf16/surrogate.utf8.txt utf8-to-utf16/invalid.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf8.truncated
    COMMAND tester utf8 utf8-to-utf16/truncated.utf8.txt utf8-to-utf16/invalid.utf16.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

## One-Way: UTF-16 -> UTF-8

add_test(
    NAME utf16.unmatched_high
    COMMAND tester utf16 utf16-to-utf8/unmatched_high.utf16.txt utf16-to-utf8/invalid.utf8.txt
    WORKING_DIRECTORY ${TEST_DIR}
)

add_test(
    NAME utf16.unmatched_low
    COMMAND tester utf16 utf16-to-utf8/unmatched_low.utf16.txt utf16-to-utf8/invalid.utf8.txt
    WORKING_DIRECTORY ${TEST_DIR}
)
