cmake_minimum_required(VERSION 3.23)

# Set compile commands export and C standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define project
project(learn_3d_computer_graphics_programming C)

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
    src/geometry_cube.h
    src/geometry_cube.c
    src/mesh.h
    src/mesh.c
    src/swap.h
    src/swap.c
    src/triangle.h
    src/triangle.c
    src/matrix.h
    src/matrix.c
    src/light.h
    src/light.c
    src/texture.h
    src/texture.c
    src/texture_redbrick.h
    src/texture_redbrick.c
    src/upng.h
    src/upng.c
    src/camera.h
    src/camera.c
    src/clipping.h
    src/clipping.c
    src/main.c
)

# Add executable
add_executable(learn_3d_computer_graphics_programming ${src_files})

# Find and link SDL2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(
    learn_3d_computer_graphics_programming
    PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
)
