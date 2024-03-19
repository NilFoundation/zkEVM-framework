# SSZ++ requires C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Hashtree REQUIRED)
find_package(sszpp REQUIRED)

# Add SSZ-dependent test
# .cpp file must have the name of target
# Handles proper linking for SSZ-dependent tests
function(add_ssz_test target)
    add_executable(${target} ${target}.cpp)

    target_link_libraries(${target} INTERFACE sszpp)
    target_link_libraries(${target} PRIVATE hashtree)
    target_link_libraries(${target} PRIVATE GTest::gtest_main)

    gtest_discover_tests(${target})
endfunction()

add_ssz_test(fixed_size_serialization)
add_ssz_test(variable_size_serialization)
