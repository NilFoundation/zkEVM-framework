set(TARGET_NAME emulator_wrapper)
set(SOURCES
    wrapper.cpp
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(${TARGET_NAME} SHARED ${SOURCES})

target_include_directories(${TARGET_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../assigner/include)

target_compile_options(${TARGET_NAME} PRIVATE "-fPIC")

find_package(evm-assigner REQUIRED)
target_link_libraries(${TARGET_NAME}
    PRIVATE
    evm-assigner::evm-assigner
    zkEVMAssignerRunner
    zkEVMDataTypes
    zkEVMJsonHelpers
    zkEVMBlockGenerator
)

install(TARGETS ${TARGET_NAME})
