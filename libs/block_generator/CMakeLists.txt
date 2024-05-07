find_package(valijson REQUIRED)

# Generate include file to use JSON Schema in source code
set(SCHEMA_STRING_FILE_NAME include/schema.json.quoted)
add_custom_command(
        OUTPUT ${SCHEMA_STRING_FILE_NAME}
        COMMAND cmake -E echo "'R\"('" > ${SCHEMA_STRING_FILE_NAME}
        COMMAND cmake -E cat ${CMAKE_CURRENT_SOURCE_DIR}/resources/schema.json >> ${SCHEMA_STRING_FILE_NAME}
        COMMAND cmake -E echo "')\"'" >> ${SCHEMA_STRING_FILE_NAME}
        DEPENDS resources/schema.json
)
add_custom_target(schema_string_file DEPENDS ${SCHEMA_STRING_FILE_NAME})

add_library(zkEVMBlockGenerator STATIC block_generator.cpp)
add_dependencies(zkEVMBlockGenerator schema_string_file)

find_package(Boost COMPONENTS REQUIRED json)
target_link_libraries(zkEVMBlockGenerator PUBLIC zkEVMDataTypes ${Boost_LIBRARIES}
                                          PRIVATE valijson)
target_include_directories(zkEVMBlockGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/include)

install(TARGETS zkEVMBlockGenerator
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
