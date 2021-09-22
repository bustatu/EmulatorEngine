add_library(CHIP8
    "chip8.cpp"
    "components/input.cpp"
    "components/graphics.cpp"
    "components/interpreter/vm.cpp"
    "components/dynarec/vm.cpp"
    "components/dynarec/cache.cpp"
)

# SDL2 stuff
find_package(SDL2 REQUIRED)
find_package(xbyak REQUIRED)

# Include directories
target_include_directories(CHIP8 PUBLIC ${SDL2_INCLUDE_DIR})

# Link external libraries
target_link_libraries(
    CHIP8
    xbyak::xbyak
    SDL2::SDL2
    SDL2::SDL2main
)