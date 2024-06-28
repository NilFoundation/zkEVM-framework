add_library(zkEVMBlockExtractor SHARED block_extractor.cpp)

target_link_libraries(zkEVMBlockExtractor PUBLIC zkEVMDataTypes)
target_include_directories(zkEVMBlockExtractor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)

install(TARGETS zkEVMBlockExtractor
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
