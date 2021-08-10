add_library(GUI
    "text/font.cpp"
    "text/text.cpp"
)

target_include_directories(GUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})