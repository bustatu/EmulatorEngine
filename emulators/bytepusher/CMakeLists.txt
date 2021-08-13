add_library(Bytepusher
    "bytepusher.cpp"
    "components/vm.cpp"
)

target_include_directories(Bytepusher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})