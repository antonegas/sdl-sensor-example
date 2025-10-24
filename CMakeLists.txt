cmake_minimum_required(VERSION 3.5)
project(example)

# Create an option to switch between a system sdl library and a vendored SDL library
option(EXAMPLE_VENDORED "Use vendored libraries" OFF)

if(EXAMPLE_VENDORED)
    # This assumes you have added SDL as a submodule in vendored/SDL
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package,
    # 2. look for the SDL3-shared component, and
    # 3. fail if the shared component cannot be found.
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

# On Windows, copy SDL3.dll to the build directory
if(WIN32)
    add_custom_command(
        TARGET example POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:example>
        VERBATIM
    )
endif()

# Generate .html file for web
if(EMSCRIPTEN)
    set_property(TARGET ${TARGET} PROPERTY SUFFIX ".html")
endif()

target_sources(example PRIVATE main.c)

# Create your game executable target as usual
add_executable(example)

# Link to the actual SDL3 library.
target_link_libraries(example PRIVATE SDL3::SDL3)
