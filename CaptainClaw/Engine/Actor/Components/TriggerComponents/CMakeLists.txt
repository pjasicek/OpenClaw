cmake_minimum_required(VERSION 3.5)

target_sources(captainclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/TriggerComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TriggerComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SoundTriggerComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SoundTriggerComponent.cpp
)
