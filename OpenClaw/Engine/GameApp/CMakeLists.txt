cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/BaseGameApp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/BaseGameLogic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CommandHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GameSaves.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MainLoop.h
    ${CMAKE_CURRENT_SOURCE_DIR}/BaseGameApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BaseGameLogic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CommandHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameSaves.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainLoop.cpp
)
