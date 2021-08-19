cmake_minimum_required(VERSION 3.5)

project(EmuEngine)

# Set custom module path
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

# Compile the currently implemented emulators
add_subdirectory("./emulators/bytepusher")
add_subdirectory("./emulators/gameboy")
add_subdirectory("./emulators/chip8")
add_subdirectory("./emulators/nes")

# Compile the main engine stuff
add_subdirectory("./menus")
add_subdirectory("./gui")
add_executable(EmuEngine
    "main.cpp"
    "state.cpp"
    "window.cpp"
    "audio.cpp"
)
target_link_libraries(EmuEngine Menus)
target_link_libraries(EmuEngine GUI)

# Link the currently implemented emulators
target_link_libraries(EmuEngine NES)
target_link_libraries(EmuEngine CHIP8)
target_link_libraries(EmuEngine Gameboy)
target_link_libraries(EmuEngine Bytepusher)

# Link SDL2 stuff
find_package(SDL2 REQUIRED)
find_package(SDL2TTF REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
target_link_libraries(EmuEngine SDL2 ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY})

# Copy the data directory to the build path
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/data/)
    add_custom_command(TARGET EmuEngine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:EmuEngine>/data/)
endif()