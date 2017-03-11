cmake_minimum_required(VERSION 3.5)

target_sources(captainclaw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawPhysics.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CollisionBody.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PhysicsContactListener.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PhysicsDebugDrawer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ClawPhysics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CollisionBody.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PhysicsContactListener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PhysicsDebugDrawer.cpp
)
