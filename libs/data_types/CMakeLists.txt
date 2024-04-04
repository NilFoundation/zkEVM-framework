# SSZ++ requires C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Hashtree REQUIRED)
find_package(sszpp REQUIRED)

set(LIBRARY_NAME zkEVMDataTypes)
set(SOURCES
    src/account.cpp
    src/account_block.cpp
    src/block.cpp
    src/block_header.cpp
    src/message.cpp
    src/state.cpp
    src/transaction.cpp
    src/transaction_receipt.cpp)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})

set(PUBLIC_HEADERS_PREFIX zkevm_framework/data_types)
set(PUBLIC_HEADERS
    include/${PUBLIC_HEADERS_PREFIX}/account.hpp
    include/${PUBLIC_HEADERS_PREFIX}/account_block.hpp
    include/${PUBLIC_HEADERS_PREFIX}/base.hpp
    include/${PUBLIC_HEADERS_PREFIX}/block.hpp
    include/${PUBLIC_HEADERS_PREFIX}/block_header.hpp
    include/${PUBLIC_HEADERS_PREFIX}/message.hpp
    include/${PUBLIC_HEADERS_PREFIX}/mpt.hpp
    include/${PUBLIC_HEADERS_PREFIX}/state.hpp
    include/${PUBLIC_HEADERS_PREFIX}/transaction.hpp
    include/${PUBLIC_HEADERS_PREFIX}/transaction_receipt.hpp)

set_target_properties(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(${LIBRARY_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(${LIBRARY_NAME} INTERFACE sszpp)
target_link_libraries(${LIBRARY_NAME} PRIVATE hashtree)

install(TARGETS ${LIBRARY_NAME}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PUBLIC_HEADERS_PREFIX}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
