#ifndef CPPMD2VIEWER_GLOBALSPACEOBJ_H
#define CPPMD2VIEWER_GLOBALSPACEOBJ_H

#include <array>
#include "MatObj.h"

class GlobalSpaceObj {
public:
    std::array<float, 16> mProjectionMat= Mat44::IDENTITY;
    std::array<float, 16> mViewMat      = Mat44::IDENTITY;
    std::array<float, 16> mModelMat     = Mat44::IDENTITY;
    std::array<float, 16> mMvpMat       = Mat44::IDENTITY;
    std::array<float, 16> mNormalMatrix = Mat44::IDENTITY;
    std::array<float,  3> mCameraPos = {};
    std::array<float,  3> mTargetPos = {};
    std::array<float,  3> mUpPos     = {};
    float mScale  = 0;
    float mRotatex= 0;
    float mRotatey= 0;
};

#endif //CPPMD2VIEWER_GLOBALSPACEOBJ_H