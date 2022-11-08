#ifndef CPPMD2VIEWER_GLOBALSPACEOBJ_H
#define CPPMD2VIEWER_GLOBALSPACEOBJ_H

#include "glm/glm.hpp"

class GlobalSpaceObj {
public:
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_vpmat;  /* = m_projection * m_view */
    glm::vec3 m_camPos;
    glm::vec3 m_targetPos;
    glm::vec3 m_up;
};

#endif //CPPMD2VIEWER_GLOBALSPACEOBJ_H