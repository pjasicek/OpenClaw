cmake_minimum_required(VERSION 3.2)

target_compile_definitions(openclaw PRIVATE SDL_MAIN_HANDLED)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawEvents.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawGameApp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawGameLogic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawHumanView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawEvents.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawGameApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawGameLogic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawHumanView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_subdirectory(Engine)
