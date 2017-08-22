cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CrumblingPegAIComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ProjectileAIComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TogglePegAIComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CrumblingPegAIComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProjectileAIComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TogglePegAIComponent.cpp
)
