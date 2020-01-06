cmake_minimum_required(VERSION 3.2)

project(tinyxml VERSION 2.6.2 LANGUAGES CXX)

add_library(tinyxml STATIC
        tinyxml.h
        tinyxml.cpp
        tinyxmlerror.cpp
        tinyxmlparser.cpp
        )

target_compile_definitions(tinyxml
        PRIVATE
        TIXML_USE_STL
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        INTERFACE
        TIXML_USE_STL
        )
