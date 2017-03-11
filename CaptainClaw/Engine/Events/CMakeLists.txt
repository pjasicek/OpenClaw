cmake_minimum_required(VERSION 3.5)

target_sources(captainclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/EventMgr.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EventMgrImpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Events.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EventMgr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventMgrImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Events.cpp
)
