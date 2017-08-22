cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/EndLevelScoreScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EndLevelScoreScreen.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ScoreScreenCommon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScoreScreenCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ScoreScreenProcesses.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScoreScreenProcesses.h
)
