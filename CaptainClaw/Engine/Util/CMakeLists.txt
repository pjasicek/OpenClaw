cmake_minimum_required(VERSION 3.5)

target_sources(captainclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Profilers.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Singleton.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StringUtil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Subject.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Profilers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StringUtil.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Converters.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Converters.cpp
)
