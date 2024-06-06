include(SchemaHelper)
generate_schema_include(block_schema_include resources/block_schema.json)

add_library(zkEVMBlockGenerator SHARED block_generator.cpp)
add_dependencies(zkEVMBlockGenerator block_schema_include)

find_package(Boost COMPONENTS REQUIRED json)
target_link_libraries(zkEVMBlockGenerator PUBLIC zkEVMDataTypes ${Boost_LIBRARIES} zkEVMJsonHelpers)
target_include_directories(zkEVMBlockGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/include)

install(TARGETS zkEVMBlockGenerator
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
