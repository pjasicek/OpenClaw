cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Console.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GameHUD.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HumanView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MovementController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/UserInterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Console.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameHUD.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HumanView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MovementController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UserInterface.cpp
)

add_subdirectory(ScoreScreen)
add_subdirectory(Touch)