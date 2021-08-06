cmake_minimum_required(VERSION 3.5)

project(EmuEngine)

# Use modified SDL2 modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${EmuEngine_SOURCE_DIR}/cmake")

set(EMU_CHIP8 true)
set(EMU_BYTEPUSHER true)

if(EMU_PS1)
    add_subdirectory("./emulators/ps1")
endif()

if(EMU_BYTEPUSHER)
    add_subdirectory("./emulators/bytepusher")
endif()

if(EMU_CHIP8)
    add_subdirectory("./emulators/chip8")
endif()

add_executable(EmuEngine
    "main.cpp"
    "state.cpp"
    "window.cpp"
    "audio.cpp"
)

if(EMU_CHIP8)
    target_link_libraries(EmuEngine CHIP8)
endif()

if(EMU_BYTEPUSHER)
    target_link_libraries(EmuEngine BYTEPUSHER)
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(EmuEngine SDL2 ${SDL2_LIBRARY})

# Copy the data directory to the build path
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/data/)
    add_custom_command(TARGET EmuEngine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:EmuEngine>/data/)
endif()