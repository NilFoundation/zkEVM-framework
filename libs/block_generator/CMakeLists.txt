add_library(zkEVMBlockGenerator STATIC block_generator.cpp)

find_package(Boost COMPONENTS REQUIRED json)
target_link_libraries(zkEVMBlockGenerator PUBLIC zkEVMDataTypes ${Boost_LIBRARIES})
target_include_directories(zkEVMBlockGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS zkEVMBlockGenerator
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/block_generator.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zkevm_framework/block_generator)
