# SSZ++ requires C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Hashtree REQUIRED)
find_package(sszpp REQUIRED)

set(LIBRARY_NAME zkEVMDataTypes)
set(SOURCES
    src/account.cpp
    src/block.cpp
    src/block_header.cpp
    src/state.cpp
    src/transaction.cpp
    src/transaction_receipt.cpp)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(${LIBRARY_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${LIBRARY_NAME} INTERFACE sszpp)
target_link_libraries(${LIBRARY_NAME} PRIVATE hashtree)
