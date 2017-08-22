cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/TriggerComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TriggerComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SoundTriggerComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SoundTriggerComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BossStagerTriggerComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/BossStagerTriggerComponent.cpp
)
