set(LIBRARY_NAME zkEVMDataTypes)

add_library(${LIBRARY_NAME} STATIC)

# SSZ++ requires C++23, also DataTypes is using <expected>
# This feature will be propagated to all targets linked with the library
target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_23)

add_subdirectory(src)

set(PUBLIC_HEADERS_PREFIX zkevm_framework/data_types)
set(PUBLIC_HEADERS
    include/${PUBLIC_HEADERS_PREFIX}/account.hpp
    include/${PUBLIC_HEADERS_PREFIX}/account_block.hpp
    include/${PUBLIC_HEADERS_PREFIX}/base.hpp
    include/${PUBLIC_HEADERS_PREFIX}/block.hpp
    include/${PUBLIC_HEADERS_PREFIX}/block_header.hpp
    include/${PUBLIC_HEADERS_PREFIX}/errors.hpp
    include/${PUBLIC_HEADERS_PREFIX}/message.hpp
    include/${PUBLIC_HEADERS_PREFIX}/mpt.hpp
    include/${PUBLIC_HEADERS_PREFIX}/state.hpp
    include/${PUBLIC_HEADERS_PREFIX}/transaction.hpp
    include/${PUBLIC_HEADERS_PREFIX}/transaction_receipt.hpp)

set_target_properties(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(${LIBRARY_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(TARGETS ${LIBRARY_NAME}
    EXPORT ${PROJECT_NAME}Targets
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PUBLIC_HEADERS_PREFIX}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
