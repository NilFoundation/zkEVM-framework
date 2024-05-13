set(TARGET_NAME assigner)
set(SOURCES
    src/main.cpp
    src/utils.cpp
    src/state_parser.cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${TARGET_NAME} ${SOURCES})

include(SchemaHelper)
generate_schema_include(state_schema_include resources/state_schema.json)

target_include_directories(${TARGET_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include)
add_dependencies(${TARGET_NAME} state_schema_include)

target_link_libraries(${TARGET_NAME} PRIVATE evmone evmone_assigner zkEVMDataTypes zkEVMJsonHelpers ${Boost_LIBRARIES})

install(TARGETS ${TARGET_NAME})
