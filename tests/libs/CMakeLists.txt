option(ENABLE_DATA_TYPES_TESTS "Enable Data Types tests" TRUE)

if(ENABLE_DATA_TYPES_TESTS)
    add_subdirectory(data_types)
endif()
