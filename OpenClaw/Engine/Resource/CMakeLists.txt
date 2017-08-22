cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceCache.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceCache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceMgr.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceMgr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Miniz.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Miniz.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ZipFile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ZipFile.cpp
)

add_subdirectory(Loaders)
