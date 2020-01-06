cmake_minimum_required(VERSION 3.2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Build_Release")

project(MidiProc)

add_executable(MidiProc WIN32 "")
target_sources(MidiProc
        PRIVATE
        main.cpp
        midiproc_s.c)

target_include_directories(MidiProc PRIVATE ../ThirdParty/)
target_link_directories(MidiProc PRIVATE ../Build_Release)
target_link_directories(MidiProc PRIVATE ../ThirdParty/SDL2/lib/x86)

target_link_libraries(MidiProc
        PRIVATE
        SDL2
        SDL2_mixer
        rpcrt4)