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
    void OnDrawFrame(std::map<std::string, Md2Model> &md2models);
};
