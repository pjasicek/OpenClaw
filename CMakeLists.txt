cmake_minimum_required(VERSION 3.5)

project(CaptainClaw)

#set(CMAKE_CXX_STANDARD 11)

add_subdirectory(Box2D)
add_subdirectory(libwap)

include_directories(./Box2D/)
include_directories(./libwap/)
include_directories(./ThirdParty/)

link_directories(./libwap)
link_directories(./Box2D)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../Build_Release)

add_executable(captainclaw "")
add_subdirectory(CaptainClaw)

target_link_libraries(captainclaw
    libwap
    tinyxml
    Box2D
    SDL2
    SDL2_mixer
    SDL2_ttf
    SDL2_image
    SDL2_gfx
)
