option(ENABLE_DATA_TYPES_TESTS "Enable Data Types tests" TRUE)
option(ENABLE_BLOCK_GENERATOR_TESTS "Enable block generator tests" TRUE)

if(ENABLE_DATA_TYPES_TESTS)
    add_subdirectory(data_types)
endif()

if (ENABLE_BLOCK_GENERATOR_TESTS)
    add_subdirectory(block_generator)
endif()
