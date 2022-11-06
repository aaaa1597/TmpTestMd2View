#pragma once

#include <map>
#include <string>
#include <memory>
#include <jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <math.h>
#include "glm/glm.hpp"
#include "Md2Obj.h"

class Renderer {
public:
    Renderer();

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame(std::map<std::string, Md2Model> &md2models);

    void SetRotationAngles(float width, float height);

    void SetScale(float scale);

//    private:
//        glm::mat4 m_projection;
//        glm::mat4 m_view;
//        glm::mat4 m_model;
//        glm::vec3 m_camPos;
//        glm::vec3 m_targetPos;
//        glm::vec3 m_up;
//        size_t m_renderFrame = 0;
//        float m_interpolation = 0.0f;
//        float m_rotationX = 0.0f;
//        float m_rotationY = 0.0f;
//        float m_xAngle = 180.0f;
//        float m_yAngle = 0.0f;
//        float m_scale = 1.0f;
};
