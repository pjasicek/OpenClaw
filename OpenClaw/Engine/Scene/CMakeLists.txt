cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorSceneNode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HUDSceneNode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SceneNodes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TilePlaneSceneNode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorSceneNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HUDSceneNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SceneNodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TilePlaneSceneNode.cpp
)
