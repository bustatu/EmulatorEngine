add_library(Gameboy
    "gameboy.cpp"
    "components/cpu/cpu.cpp"
    "components/bus/bus.cpp"
    "components/rom/bios.cpp"
)

target_include_directories(Gameboy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})