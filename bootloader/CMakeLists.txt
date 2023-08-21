cmake_minimum_required(VERSION 3.22)
project(bootloader)

# Run the makefile in the bootloader directory
add_custom_target(bootloader
    COMMAND $(MAKE) build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Copy bin/bootloader to "build" directory
add_custom_command(TARGET bootloader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/bin/bootloader.bin
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bootloader.bin
)
