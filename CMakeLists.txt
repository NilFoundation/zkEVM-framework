cmake_minimum_required(VERSION 3.28)
project(zkEVM-framework)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

option(ENABLE_TESTS "Enable tests" FALSE)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
