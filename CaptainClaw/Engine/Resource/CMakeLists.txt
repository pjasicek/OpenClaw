cmake_minimum_required(VERSION 3.5)

target_sources(captainclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceCache.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceCache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceMgr.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceMgr.cpp
)

add_subdirectory(Loaders)
