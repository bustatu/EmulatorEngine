add_library(NES
    "nes.cpp"
    "components/bus/bus.cpp"
    "components/cpu/cpu.cpp"
    "components/rom/rom.cpp"
)

target_include_directories(NES PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})