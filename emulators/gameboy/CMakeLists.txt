add_library(Gameboy
    "gameboy.cpp"
    "components/bios/bios.cpp"
    "components/bus/bus.cpp"
    "components/cpu/cpu.cpp"
    "components/gpu/gpu.cpp"
    "components/ram/ram.cpp"
    "components/rom/rom.cpp"
    "components/rom/MBCs/mbc0.cpp"
    "components/rom/MBCs/mbc1.cpp"
    "components/timer/timer.cpp"
    "components/joypad/joypad.cpp"
)

# SDL2 stuff
find_package(SDL2 REQUIRED)

# Include directories
target_include_directories(Gameboy PUBLIC ${SDL2_INCLUDE_DIR})

# Link external libraries
target_link_libraries(
    Gameboy
    SDL2::SDL2
    SDL2::SDL2main
)