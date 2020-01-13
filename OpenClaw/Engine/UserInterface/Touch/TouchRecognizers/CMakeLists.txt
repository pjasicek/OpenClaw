cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRecognizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TapRecognizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/JoystickRecognizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SwipeRecognizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PressRecognizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRecognizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TapRecognizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JoystickRecognizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SwipeRecognizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PressRecognizer.cpp
)
