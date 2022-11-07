#ifndef CPPMD2VIEWER_GLOBALSPACEOBJ_H
#define CPPMD2VIEWER_GLOBALSPACEOBJ_H

#include <array>
#include "MatObj.h"
#include "glm/glm.hpp"

class GlobalSpaceObj {
public:
    std::array<float, 16> mProjectionMat= {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
    };
    std::array<float, 16> mViewMat      =  {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
    };
    std::array<float, 16> mModelMat     =  {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
    };
    std::array<float, 16> mMvpMat       =  {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
    };
    std::array<float, 16> mNormalMatrix =  {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
    };
    std::array<float,  3> mCameraPos = {};
    std::array<float,  3> mTargetPos = {};
    std::array<float,  3> mUpPos     = {};
    float mScale  = 1.0f;
    float mRotatex= 180.0f;
    float mRotatey= 0;

public:
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_model;
    glm::vec3 m_camPos;
    glm::vec3 m_targetPos;
    glm::vec3 m_up;
};

#endif //CPPMD2VIEWER_GLOBALSPACEOBJ_H