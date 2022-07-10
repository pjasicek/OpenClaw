cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/EnemyAIComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EnemyAIStateComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EnemyAIComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EnemyAIStateComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PunkRatAIStateComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PunkRatAIStateComponent.h
)

add_subdirectory(Gabriel)
add_subdirectory(Marrow)
add_subdirectory(Aquatis)
add_subdirectory(RedTail)
