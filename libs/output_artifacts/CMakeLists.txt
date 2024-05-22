find_package(Boost COMPONENTS REQUIRED log log_setup program_options regex)

set(LIBRARY_NAME zkEVMOutputArtifacts)

add_library(${LIBRARY_NAME} STATIC src/output_artifacts.cpp)

target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_23)

target_link_libraries(${LIBRARY_NAME} PRIVATE ${Boost_LIBRARIES})

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ${LIBRARY_NAME})
