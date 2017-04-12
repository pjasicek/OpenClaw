cmake_minimum_required(VERSION 3.2)

option(Android "Android" OFF)

project(CaptainClaw)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

add_subdirectory(Box2D)
add_subdirectory(libwap)

#if(Android)
#    add_subdirectory(./ThirdParty/Tinyxml)
#    add_subdirectory(./ThirdParty/SDL2-2.0.5)
#    include_directories(./ThirdParty/SDL2-2.0.5/include/)
#    link_directories(./ThirdParty/SDL2-2.0.5/libs/armeabi/)
#    add_subdirectory(./ThirdParty/SDL2_gfx-1.0.1)
#endif(Android)

include_directories(./Box2D/)
include_directories(./libwap/)
include_directories(./ThirdParty/)

# for android
#include_directories(./ThirdParty/Tinyxml/)

link_directories(./libwap)
link_directories(./Box2D)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../Build_Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../android/libs/armeabi-v7a)

if(Android)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID")
    add_library(captainclaw SHARED "")
else (Android)
    add_executable(captainclaw "")
endif(Android)

add_subdirectory(CaptainClaw)

list(APPEND TARGET_LIBS
    libwap
    tinyxml
    Box2D
    SDL2
    SDL2_mixer
    SDL2_ttf
    SDL2_image
    SDL2_gfx
)

if(Android)
    list(APPEND TARGET_LIBS
        GLESv2
        GLESv1_CM
        dl
        log
        android
    )
endif(Android)

target_link_libraries(captainclaw ${TARGET_LIBS})
