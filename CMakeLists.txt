cmake_minimum_required(VERSION 3.22)
project(8086)

# If "build" directory doesn't exist, create it
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/build)
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
endif()

# Set variable for build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_subdirectory(bootloader)
add_subdirectory(emulator)

add_dependencies(8086_emulator bootloader)