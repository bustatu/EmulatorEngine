cmake_minimum_required(VERSION 3.5)

project(EmuEngine)

# Set custom module path
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

# Build options
option(USE_EXTERNAL_JSON "Use an external JSON library" OFF)

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
)

# Link main engine stuff
target_link_libraries(EmuEngine Menus GUI)

# Link currently implemented emulators
target_link_libraries(EmuEngine NES CHIP8 Gameboy Bytepusher)

# Link external libraries
target_link_libraries(
    EmuEngine
    nlohmann_json::nlohmann_json
    SDL2 ${SDL2_LIBRARY}
    ${SDL2TTF_LIBRARY}
)


# Copy the data directory to the build path
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/data/)
    add_custom_command(TARGET EmuEngine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:EmuEngine>/data/)
endif()