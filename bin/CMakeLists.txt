find_package(Boost COMPONENTS REQUIRED filesystem log log_setup program_options thread system json)

add_subdirectory(assigner)
add_subdirectory(block_generator)
add_subdirectory(emulator)
