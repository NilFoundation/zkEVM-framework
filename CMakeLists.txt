cmake_minimum_required(VERSION 3.28)
project(zkEVM-framework)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

option(BUILD_DOCS "Build documentation" FALSE)
option(ENABLE_TESTS "Enable tests" FALSE)

if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    doxygen_add_docs(docs COMMENT "Generating API documentation with Doxygen")
endif()

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
