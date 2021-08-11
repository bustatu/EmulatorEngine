add_library(Gameboy
    "gameboy.cpp"
    "components/bus/bus.cpp"
    "components/cpu/cpu.cpp"
    "components/ram/ram.cpp"
    "components/rom/bios.cpp"
)

target_include_directories(Gameboy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})