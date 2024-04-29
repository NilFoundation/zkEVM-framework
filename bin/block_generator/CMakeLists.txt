add_executable(block_gen src/main.cpp)
target_link_libraries(block_gen zkEVMBlockGenerator ${Boost_LIBRARIES})

install(TARGETS block_gen DESTINATION ${CMAKE_INSTALL_BINDIR})
