set(TARGET_NAME emulator_wrapper)
set(SOURCES
    wrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../assigner/src/utils.cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(${TARGET_NAME} SHARED ${SOURCES})

target_include_directories(${TARGET_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../assigner/include)

target_compile_options(${TARGET_NAME} PRIVATE "-fPIC")

target_link_libraries(${TARGET_NAME}
    PRIVATE
    evmone
    evmone_assigner
    zkEVMDataTypes
    zkEVMJsonHelpers
    zkEVMOutputArtifacts
    zkEVMBlockGenerator
    ${Boost_LIBRARIES})

install(TARGETS ${TARGET_NAME})
