add_library(Gameboy
    "gameboy.cpp"
    "components/bios/bios.cpp"
    "components/bus/bus.cpp"
    "components/cpu/cpu.cpp"
    "components/gpu/gpu.cpp"
    "components/ram/ram.cpp"
    "components/rom/rom.cpp"
)

target_include_directories(Gameboy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})