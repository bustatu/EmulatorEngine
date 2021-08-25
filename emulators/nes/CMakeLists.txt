add_library(NES
    "nes.cpp"
    "components/bus/bus.cpp"
    "components/cpu/cpu.cpp"
    "components/rom/rom.cpp"
)

# SDL2 stuff
find_package(SDL2 REQUIRED)

# Include directories
target_include_directories(NES PUBLIC ${SDL2_INCLUDE_DIR})

# Link external libraries
target_link_libraries(
    NES
    SDL2::SDL2
    SDL2::SDL2main
)