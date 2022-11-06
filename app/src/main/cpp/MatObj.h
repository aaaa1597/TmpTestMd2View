#ifndef CPPMD2VIEWER_MATOBJ_H
#define CPPMD2VIEWER_MATOBJ_H

#include <array>

static const float PI = 3.141592653589793;  /* NASA 15桁 */

class Mat44 {
public:
    static const std::array<float, 16> IDENTITY;

public:
    static std::array<float, 16> getPerspectivef(float fovy, float aspect, float near, float far);
    static std::array<float, 16> getLookAtf(const std::array<float, 3> &camerapos, const std::array<float, 3> &targetpos, const std::array<float, 3> &uppos);
    static std::array<float, 16> getRotatef(float angle, float x, float y, float z);
    static std::array<float, 16> multMatrixf(const std::array<float, 16> &a, const std::array<float, 16> &m);
    static std::array<float, 16> translatef(const std::array<float, 16> &mat, const std::array<float, 3> &vec);
    static std::array<float, 16> scalef(const std::array<float, 16> &mat, const std::array<float, 3> &vec);
    static std::array<float, 16> invertf(const std::array<float, 16> &mat);
    static std::array<float, 16> transposef(const std::array<float, 16> &mat);
};

class Vec3 {
public:
    static std::array<float, 3> normalize(const std::array<float, 3> &vec);
    static std::array<float, 3> cross(const std::array<float, 3> &v1, const std::array<float, 3> &v2);
};

#endif //CPPMD2VIEWER_MATOBJ_H
