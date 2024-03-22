# SSZ++ requires C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add test for DataTypes library
# .cpp file must have the name of target
function(add_data_types_test target)
    add_executable(${target} ${target}.cpp)

    target_link_libraries(${target} PRIVATE zkEVMDataTypes)
    target_link_libraries(${target} PRIVATE GTest::gtest_main)

    gtest_discover_tests(${target})
endfunction()

add_data_types_test(test_account)
add_data_types_test(test_block)
add_data_types_test(test_block_header)
add_data_types_test(test_state)
add_data_types_test(test_transaction)
add_data_types_test(test_transaction_receipt)
