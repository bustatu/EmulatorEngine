cmake_minimum_required(VERSION 3.13)
project(EmuEngine)

set(SOURCE_FILES
    src/main.cpp
    src/engine/window.cpp
    src/engine/audio.cpp
    src/engine/renderer.cpp
    src/emulators/bytepusher/bytepusher.cpp
)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(EmuEngine ${SOURCE_FILES})

# Dependencies

# Use this on Windows to set the SDL path
# set(SDL2_DIR "<path>")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(EmuEngine ${SDL2_LIBRARIES})