cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/PowerupProcess.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Process.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ProcessMgr.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PowerupProcess.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Process.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProcessMgr.cpp
)
