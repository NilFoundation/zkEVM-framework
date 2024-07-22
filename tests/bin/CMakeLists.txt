option(ENABLE_EXECUTABLES_TESTS "Enable tests of executables" TRUE)

if(ENABLE_EXECUTABLES_TESTS)
    add_custom_target(executables_tests)

    add_custom_target(test_run_block_gen
        COMMAND $<TARGET_FILE:block_gen>
        -i ${CMAKE_SOURCE_DIR}/bin/assigner/example_data/call_block.json
        -o ${CMAKE_CURRENT_BINARY_DIR}/call_block.ssz
        COMMENT "Running test of block generator executable"
        DEPENDS
        $<TARGET_FILE:block_gen>
        ${CMAKE_SOURCE_DIR}/bin/assigner/example_data/state.json
        VERBATIM)
    add_dependencies(executables_tests test_run_block_gen)

    add_custom_target(test_run_assigner
        COMMAND $<TARGET_FILE:assigner>
        -b ${CMAKE_CURRENT_BINARY_DIR}/call_block.ssz
        -t assignments
        -e pallas
        -s ${CMAKE_SOURCE_DIR}/bin/assigner/example_data/state.json
        --log-level debug
        COMMENT "Running test of assigner executable"
        DEPENDS
        $<TARGET_FILE:assigner>
        test_run_block_gen
        ${CMAKE_SOURCE_DIR}/bin/assigner/example_data/state.json
        VERBATIM)
    add_dependencies(executables_tests test_run_assigner)
endif()
