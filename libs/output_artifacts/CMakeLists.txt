find_package(Boost COMPONENTS REQUIRED program_options regex)

set(LIBRARY_NAME zkEVMOutputArtifacts)

add_library(${LIBRARY_NAME} SHARED src/output_artifacts.cpp)

target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_23)

target_link_libraries(${LIBRARY_NAME} PUBLIC Boost::program_options PRIVATE Boost::regex)

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ${LIBRARY_NAME})
