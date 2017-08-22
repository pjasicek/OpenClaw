cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/AniLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/DefaultLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MidiLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PalLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PcxLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PidLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WavLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WwdLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/XmlLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PcxLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PngLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AniLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MidiLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PalLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PidLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WavLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WwdLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/XmlLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PcxLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PngLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceCorrection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceCorrection.h
)
