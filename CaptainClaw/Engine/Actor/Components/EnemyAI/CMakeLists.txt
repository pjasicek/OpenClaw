cmake_minimum_required(VERSION 3.5)

target_sources(captainclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/EnemyAIComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EnemyAIStateComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EnemyAIComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EnemyAIStateComponent.cpp
)
