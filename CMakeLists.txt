cmake_minimum_required(VERSION 3.23)

# Set compile commands export and C standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define project
project(learn_computer_graphics C)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/x64")

# Define source files
set(src_files
    src/array.h
    src/array.c
    src/display.h
    src/display.c
    src/vector.h
    src/vector.c
    src/mesh.h
    src/mesh.c
    src/triangle.h
    src/triangle.c
    src/main.c
)

# Add executable
add_executable(learn_computer_graphics ${src_files})

# Find and link SDL2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(
    learn_computer_graphics
    PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
)
