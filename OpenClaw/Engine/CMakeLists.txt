cmake_minimum_required(VERSION 3.2)

target_sources(openclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Interfaces.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Interfaces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SharedDefines.h
    ${CMAKE_CURRENT_SOURCE_DIR}/XmlMacros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ActorDefinitions.h
)

add_subdirectory(Actor)
add_subdirectory(Audio)
add_subdirectory(Events)
add_subdirectory(GameApp)
add_subdirectory(Graphics2D)
add_subdirectory(Logger)
add_subdirectory(Physics)
add_subdirectory(Process)
add_subdirectory(Resource)
add_subdirectory(Scene)
add_subdirectory(UserInterface)
add_subdirectory(Util)
