if(NOT Boost_FOUND)
    find_package(Boost COMPONENTS REQUIRED filesystem log log_setup program_options thread system)
endif()

add_subdirectory(assigner)
