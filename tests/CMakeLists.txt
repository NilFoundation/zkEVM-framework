# GoogleTest requires C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

include(GoogleTest)

option(ENABLE_SSZ_TESTS "Enable SSZ tests" TRUE)

if(ENABLE_SSZ_TESTS)
    add_subdirectory(ssz)
endif()
