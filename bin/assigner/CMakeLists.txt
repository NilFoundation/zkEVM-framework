set(TARGET_NAME assigner)
set(SOURCES
    src/main.cpp)

add_executable(${TARGET_NAME} ${SOURCES})

target_compile_options(${TARGET_NAME} PRIVATE -std=c++23)

set(ASSIGNER_INCLUDES ${CMAKE_INSTALL_PREFIX}/include ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ASSIGNER_LIBS ${CMAKE_INSTALL_PREFIX}/lib)

target_include_directories(${TARGET_NAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../libs/data_types/include>
    $<$<BOOL:${Boost_FOUND}>:${Boost_INCLUDE_DIRS}>
    ${ASSIGNER_INCLUDES})

target_link_directories(${TARGET_NAME} PRIVATE ${ASSIGNER_LIBS})
target_link_libraries(${TARGET_NAME} PRIVATE evmone zkEVMDataTypes ${Boost_LIBRARIES})
