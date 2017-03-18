cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(libwap)

add_library(libwap STATIC "")

target_sources(libwap
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/IO.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libwap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Miniz.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AniFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Miniz.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PalFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PidFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RezArchiveFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WwdFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/XmiFile.cpp
)
