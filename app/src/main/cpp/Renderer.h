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
};
