cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_INIT                "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
project(EmuEngine)

set(SOURCE_FILES
    src/main.cpp
    src/window/window.cpp
    src/window/audio.cpp
    src/window/renderer.cpp
    src/emulators/bytepusher/bytepusher.cpp
)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(EmuEngine ${SOURCE_FILES})

# Dependencies
set(SDL2_DIR "D:/Frameworks/SDL2/")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(EmuEngine ${SDL2_LIBRARIES})