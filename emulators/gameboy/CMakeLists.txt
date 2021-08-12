add_library(Gameboy
    "gameboy.cpp"
    "components/bus/bus.cpp"
    "components/pus/gpu.cpp"
    "components/pus/cpu.cpp"
    "components/ram/ram.cpp"
    "components/rom/bios.cpp"
    "components/rom/rom.cpp"
)

target_include_directories(Gameboy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})