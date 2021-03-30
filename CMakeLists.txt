# Copyright (c) 2021 Chanjung Kim (paxbun). All rights reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.13)
project(simple-mips-emu VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library definitions
add_library(simple-mips-emu STATIC
    ${PROJECT_SOURCE_DIR}/Source/Emulation.cc
    ${PROJECT_SOURCE_DIR}/Source/Memory.cc
)
target_include_directories(simple-mips-emu PUBLIC ${PROJECT_SOURCE_DIR}/Public)

# Executable definitions
add_executable(runfile ${PROJECT_SOURCE_DIR}/Source/Main.cc)
target_link_libraries(runfile simple-mips-emu)

# Unit tests
option(ENABLE_SIMPLE_MIPS_EMU_TEST "Enable unit tests" OFF)
if (ENABLE_SIMPLE_MIPS_EMU_TEST)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    function(add_simple_mips_emu_test TEST_NAME)
        set(FILE_NAME ${TEST_NAME})
        set(EXE_NAME ${TEST_NAME})
        
        string(REGEX REPLACE "([^A-Z\-])([A-Z][A-Z]+)([A-Z][a-z])" "\\1-\\2-\\3" EXE_NAME "${EXE_NAME}")
        string(REGEX REPLACE "([A-Z]+)$" "-\\1" EXE_NAME "${EXE_NAME}")
        string(REGEX REPLACE "([^-])([A-Z][a-z]+)" "\\1-\\2" EXE_NAME "${EXE_NAME}")
        string(TOLOWER "${EXE_NAME}" EXE_NAME)
        
        add_executable(${EXE_NAME} "Tests/${FILE_NAME}.cc")
        target_link_libraries(${EXE_NAME} GTest::gtest_main simple-mips-emu)
        add_test(NAME ${TEST_NAME} COMMAND ${EXE_NAME})
        
        unset(FILE_NAME)
        unset(EXE_NAME)
        unset(TEST_NAME)
    endfunction()

    add_simple_mips_emu_test(MemoryTest)
endif()