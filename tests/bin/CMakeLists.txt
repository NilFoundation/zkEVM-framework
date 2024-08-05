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

    # Initial integration test with generating data on cluster and passing them into assigner
    # Step 1: create Nil CLI config file
    set(block_generator_script ${CMAKE_SOURCE_DIR}/bin/block_generator/block_generator.py)
    add_custom_command(
        OUTPUT test_config.yaml
        COMMAND python3
        ${block_generator_script}
        --mode make-config
        --cli-config-name ${CMAKE_CURRENT_BINARY_DIR}/test_config.yaml
        DEPENDS
        ${block_generator_script}
        COMMENT "Generating Nil CLI config"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )

    # Step 2: generate configs with block and account storage state data
    add_custom_command(
        OUTPUT legacy_config.block legacy_config.state
        COMMAND python3
        ${block_generator_script}
        --mode generate-block
        --cli-config-name ${CMAKE_CURRENT_BINARY_DIR}/test_config.yaml
        --block-config-name ${CMAKE_SOURCE_DIR}/bin/block_generator/example_data/block_config.json
        --generated-legacy-configs ${CMAKE_CURRENT_BINARY_DIR}/legacy_config
        DEPENDS
        ${block_generator_script}
        ${CMAKE_SOURCE_DIR}/bin/block_generator/example_data/block_config.json
        test_config.yaml
        COMMENT "Generating block from config"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}  # It is essential because we have relative paths in block_config.json
        VERBATIM
    )

    # Step 2: generate SSZ block from config
    add_custom_command(
        OUTPUT legacy_block.ssz
        COMMAND $<TARGET_FILE:block_gen>
        -i legacy_config.block
        -o legacy_block.ssz
        DEPENDS
        $<TARGET_FILE:block_gen>
        legacy_config.block
        COMMENT "Generating SSZ block from config"
        VERBATIM
    )

    # Step 3: run assigner
    add_custom_target(assigner_integration_test
        COMMAND $<TARGET_FILE:assigner>
        -b ${CMAKE_CURRENT_BINARY_DIR}/legacy_block.ssz
        -t assignment
        -e pallas
        -s legacy_config.state
        --log-level debug
        DEPENDS
        $<TARGET_FILE:assigner>
        legacy_config.state
        legacy_block.ssz
        COMMENT "Running assigner integration test"
        VERBATIM
    )
    add_dependencies(executables_tests assigner_integration_test)
endif()
