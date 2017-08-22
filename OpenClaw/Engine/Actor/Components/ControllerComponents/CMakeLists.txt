cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/AmmoComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HealthComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LifeComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PowerupComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ScoreComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AmmoComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HealthComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LifeComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PowerupComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScoreComponent.cpp
)
