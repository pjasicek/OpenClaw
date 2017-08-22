cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Actor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorTemplates.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Actor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorTemplates.cpp
)

add_subdirectory(Components)
