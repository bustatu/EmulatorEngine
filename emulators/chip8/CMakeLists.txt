add_library(CHIP8
    "chip8.cpp"
    "components/vm.cpp"
    "components/vm-dynarec.cpp"
    "components/input.cpp"
    "components/graphics.cpp"
)

# SDL2 stuff
find_package(SDL2 REQUIRED)

# Include directories
target_include_directories(CHIP8 PUBLIC ${SDL2_INCLUDE_DIR})

# Link external libraries
target_link_libraries(
    CHIP8
    SDL2::SDL2
    SDL2::SDL2main
)