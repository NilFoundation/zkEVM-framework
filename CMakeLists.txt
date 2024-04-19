cmake_minimum_required(VERSION 3.27)

project(zkEVM-framework VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
# Add json helpers to module path
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs/json_helpers/cmake ${CMAKE_MODULE_PATH})
include(GNUInstallDirs)

option(BUILD_DOCS "Build documentation" FALSE)
option(ENABLE_TESTS "Enable tests" FALSE)

add_subdirectory(libs)
add_subdirectory(bin)

if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    doxygen_add_docs(docs COMMENT "Generating API documentation with Doxygen")
endif()

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(EXPORT ${PROJECT_NAME}Targets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
)
