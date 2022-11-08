#ifndef CPPMD2VIEWER_GLOBALSPACEOBJ_H
#define CPPMD2VIEWER_GLOBALSPACEOBJ_H

#include "glm/glm.hpp"

class GlobalSpaceObj {
public:
    glm::mat4 mProjectionMat;
    glm::mat4 mViewMat;
    glm::mat4 mVpMat;  /* = mProjectionMat * mViewMat */
    glm::vec3 mCameraPos;
    glm::vec3 mTargetPos;
    glm::vec3 mUpPos;
};

#endif //CPPMD2VIEWER_GLOBALSPACEOBJ_H