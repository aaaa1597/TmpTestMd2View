cmake_minimum_required(VERSION 3.4.1)

project("cppmd2loader")

add_library(MD2Loader SHARED
        Renderer.cpp
        Renderer.h
        Md2Obj.cpp
        ShaderProgram.cpp
        TexObj.cpp)

add_library(jni SHARED Jni.cpp)
add_library(globj SHARED GlObj.cpp)
add_library(matobj SHARED MatObj.cpp)

include_directories(
        ./third_party_libraries)

find_library(log-lib log)

target_link_libraries(jni ${log-lib} android MD2Loader globj matobj)
target_link_libraries(
        MD2Loader
        EGL
        GLESv2
        android
        ${log-lib} globj)
target_link_libraries(globj ${log-lib} EGL GLESv2)
