option(ENABLE_DATA_TYPES_TESTS "Enable Data Types tests" TRUE)
option(ENABLE_BLOCK_GENERATOR_TESTS "Enable block generator tests" TRUE)
option(ENABLE_OUTPUT_ARTIFACTS_TESTS "Enable output artifacts tests" TRUE)
option(ENABLE_ASSIGNER_RUNNER_TESTS "Enable assigner runner tests" TRUE)

if(ENABLE_DATA_TYPES_TESTS)
    add_subdirectory(data_types)
endif()

if (ENABLE_BLOCK_GENERATOR_TESTS)
    add_subdirectory(block_generator)
endif()

if (ENABLE_OUTPUT_ARTIFACTS_TESTS)
    add_subdirectory(output_artifacts)
endif()

if (ENABLE_ASSIGNER_RUNNER_TESTS)
    add_subdirectory(assigner_runner)
endif()
