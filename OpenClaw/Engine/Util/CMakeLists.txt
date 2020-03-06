cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
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
    ${CMAKE_CURRENT_SOURCE_DIR}/PrimeSearch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PrimeSearch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/XmlUtil.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/XmlUtil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawLevelUtil.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawLevelUtil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Point.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CustomAssert.h
)
