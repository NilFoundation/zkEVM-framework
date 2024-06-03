add_library(zkEVMAssignerRunner STATIC
            src/runner.cpp
            src/utils.cpp
            src/state_parser.cpp
)

include(SchemaHelper)
generate_schema_include(state_schema_include resources/state_schema.json)
add_dependencies(zkEVMAssignerRunner state_schema_include)

target_include_directories(zkEVMAssignerRunner
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                            PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
)

find_package(Boost COMPONENTS REQUIRED log random)
target_link_libraries(zkEVMAssignerRunner
                        PUBLIC
                        evmone_assigner
                        zkEVMDataTypes
                        zkEVMJsonHelpers
                        zkEVMOutputArtifacts
                        Boost::log
                        Boost::random
                        PRIVATE
                        evmone
)

install(TARGETS zkEVMAssignerRunner
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})