# SSZ++ can be compiled only with GCC 13+
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Data types library can be built only with GCC 13+")
endif()
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13.0)
    message(FATAL_ERROR "Data types library can be built only with GCC 13+")
endif()

find_package(sszpp REQUIRED)
# Hashtree and intx are the dependencies of sszpp, but the package didn't export them properly
# So we have to find and link them manually
find_package(Hashtree REQUIRED)
find_package(intx REQUIRED)

set(SOURCES
    account.cpp
    account_block.cpp
    base.cpp
    block.cpp
    block_header.cpp
    errors.cpp
    message.cpp
    state.cpp
    transaction.cpp
    transaction_receipt.cpp)

target_sources(${LIBRARY_NAME} PRIVATE ${SOURCES})

target_link_libraries(${LIBRARY_NAME} PRIVATE sszpp::sszpp Hashtree::Hashtree intx::intx)
