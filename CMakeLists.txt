cmake_minimum_required(VERSION 3.5)

project(EmuEngine)

# Build options
option(USE_EXTERNAL_JSON "Use an external JSON library" OFF)

# Please make use of your brain if you have one, don't be me
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-operator-names")

# Add external libraries
add_subdirectory("./external")

# Add currently implemented emulators
add_subdirectory("./emulators")

# Add main engine stuff
add_subdirectory("./menus")
add_subdirectory("./gui")

# Add main executable files
add_executable(
    EmuEngine
    "main.cpp"
    "state.cpp"
    "window.cpp"
    "audio.cpp"
    "gamepad.cpp"
)

# SDL2 stuff
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)

# Dynarec stuff
find_package(xbyak CONFIG REQUIRED)

# Include directories
target_include_directories(EmuEngine PUBLIC ${SDL2_INCLUDE_DIR})

# Link external libraries
target_link_libraries(
    EmuEngine
    PRIVATE
    Menus GUI
    CHIP8 Gameboy Bytepusher
    nlohmann_json::nlohmann_json
    xbyak::xbyak
    SDL2::SDL2
    SDL2::SDL2main
    SDL2::SDL2_ttf
)

# Copy the data directory to the build path
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/data/)
    add_custom_command(TARGET EmuEngine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:EmuEngine>/data/)
endif()