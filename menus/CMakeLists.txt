add_library(Menus
    "main_menu.cpp"
)

# SDL2 stuff
find_package(SDL2 REQUIRED)
find_package(sdl2-ttf REQUIRED)

# Include directories
target_include_directories(Menus PUBLIC ${SDL2_INCLUDE_DIR})

# Link external libraries
target_link_libraries(
    Menus
    SDL2::SDL2
    SDL2::SDL2main
    SDL2::SDL2_ttf
)