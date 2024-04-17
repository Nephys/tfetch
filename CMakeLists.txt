# CMake minimum version
cmake_minimum_required(VERSION 3.18)

# LSP Support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# Project definition
project(
    tfetch
    VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
    DESCRIPTION "t(urbo)fetch a fast and lightweight fetch utility"
    LANGUAGES C
)

# Executable
add_executable(
    tfetch

    # Entry point
    "src/main.c"
)

# Debug and release macros
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(tfetch PRIVATE DEBUG)
else()
    target_compile_definitions(tfetch PRIVATE RELEASE)
endif()

# Compile options
target_compile_options(tfetch PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-ignored-qualifiers -Wno-unused-parameter -Wsign-compare)

# Install the app
install(TARGETS tfetch DESTINATION bin)
