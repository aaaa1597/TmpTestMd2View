cmake_minimum_required(VERSION 3.4.1)

project("cppmd2loader")

add_library(MD2Loader SHARED
        Renderer.cpp
        Renderer.h
        Md2Obj.cpp
        Jni.cpp
        ShaderProgram.cpp
        Texture2D.cpp
        TexObj.cpp)

include_directories(
        ./third_party_libraries)

find_library(log-lib log)

target_link_libraries(
        MD2Loader
        EGL
        GLESv2
        android
        ${log-lib})
