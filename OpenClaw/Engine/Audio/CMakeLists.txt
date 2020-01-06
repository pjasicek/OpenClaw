cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Audio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Audio.cpp
)

if (WIN32)
    target_sources(openclaw PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/midiproc_c.c)
endif (WIN32)