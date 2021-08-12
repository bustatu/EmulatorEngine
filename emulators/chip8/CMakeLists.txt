add_library(CHIP8
    "chip8.cpp"
    "components/vm.cpp"
    "components/input.cpp"
    "components/graphics.cpp"
)

target_include_directories(CHIP8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})