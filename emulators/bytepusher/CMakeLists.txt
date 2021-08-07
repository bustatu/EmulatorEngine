add_library(BytePusher
    "bytepusher.cpp"
    "components/vm.cpp"
)

target_include_directories(BytePusher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})