#include "Renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <android/log.h>
#include "GlobalSpaceObj.h"

GlobalSpaceObj                        gGlobalSpacePrm;/* グローバル空間パラメータ */

Renderer::Renderer() = default;

void Renderer::OnSurfaceChanged(int width, int height) {
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "(w,h)=(%d,%d) %s %s(%d)", width, height, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

    // Create the projection matrix
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    gGlobalSpacePrm.m_projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    gGlobalSpacePrm.m_camPos = glm::vec3(0.0f, 0.0f, 0.0f);
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "camPos-vec(%f,%f,%f) %s %s(%d)", gGlobalSpacePrm.m_camPos.x, gGlobalSpacePrm.m_camPos.y, gGlobalSpacePrm.m_camPos.z, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
    gGlobalSpacePrm.m_targetPos = glm::vec3(0.0f, 0.0f, -20.0f);
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "targetPos-vec(%f,%f,%f) %s %s(%d)", gGlobalSpacePrm.m_targetPos.x, gGlobalSpacePrm.m_targetPos.y, gGlobalSpacePrm.m_targetPos.z, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
    gGlobalSpacePrm.m_up = glm::vec3(1.0f, 0.0f, 0.0f);
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "up-vec(%f,%f,%f) %s %s(%d)", gGlobalSpacePrm.m_up.x, gGlobalSpacePrm.m_up.y, gGlobalSpacePrm.m_up.z, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

    // Create the View matrix
    gGlobalSpacePrm.m_view = glm::lookAt(gGlobalSpacePrm.m_camPos, gGlobalSpacePrm.m_camPos + gGlobalSpacePrm.m_targetPos, gGlobalSpacePrm.m_up);
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "view-mat[0](%f,%f,%f,%f) %s %s(%d)", gGlobalSpacePrm.m_view[0][0], gGlobalSpacePrm.m_view[0][1], gGlobalSpacePrm.m_view[0][2], gGlobalSpacePrm.m_view[0][3], __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "view-mat[1](%f,%f,%f,%f) %s %s(%d)", gGlobalSpacePrm.m_view[1][0], gGlobalSpacePrm.m_view[1][1], gGlobalSpacePrm.m_view[1][2], gGlobalSpacePrm.m_view[1][3], __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "view-mat[2](%f,%f,%f,%f) %s %s(%d)", gGlobalSpacePrm.m_view[2][0], gGlobalSpacePrm.m_view[2][1], gGlobalSpacePrm.m_view[2][2], gGlobalSpacePrm.m_view[2][3], __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "view-mat[3](%f,%f,%f,%f) %s %s(%d)", gGlobalSpacePrm.m_view[3][0], gGlobalSpacePrm.m_view[3][1], gGlobalSpacePrm.m_view[3][2], gGlobalSpacePrm.m_view[3][3], __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
}

void Renderer::OnDrawFrame(std::map<std::string, Md2Model> &md2models)
{
//    std::chrono::system_clock::time_point  start, end; // 型は auto で可
//    start = std::chrono::system_clock::now(); // 計測開始時間

    Md2Model *m_player = &md2models.at("female");
    Md2Model *m_player2= &md2models.at("grunt");

    static const int START_FRAME = 0;
    static const int END_FRAME = m_player->GetEndFrame();

    gGlobalSpacePrm.m_xAngle += gGlobalSpacePrm.m_rotationX;
    gGlobalSpacePrm.m_yAngle -= gGlobalSpacePrm.m_rotationY;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_player->Draw(gGlobalSpacePrm.m_renderFrame, gGlobalSpacePrm.m_xAngle, gGlobalSpacePrm.m_yAngle, gGlobalSpacePrm.m_scale, gGlobalSpacePrm.m_interpolation, gGlobalSpacePrm.m_view, gGlobalSpacePrm.m_projection, gGlobalSpacePrm.m_model);
    m_player2->Draw(gGlobalSpacePrm.m_renderFrame, gGlobalSpacePrm.m_xAngle, gGlobalSpacePrm.m_yAngle, gGlobalSpacePrm.m_scale, gGlobalSpacePrm.m_interpolation, gGlobalSpacePrm.m_view, gGlobalSpacePrm.m_projection, gGlobalSpacePrm.m_model);
    if (gGlobalSpacePrm.m_interpolation >= 1.0f)
    {
        gGlobalSpacePrm.m_interpolation = 0.0f;
        if (gGlobalSpacePrm.m_renderFrame == END_FRAME)
        {
            gGlobalSpacePrm.m_renderFrame = START_FRAME;
        }
        else
        {
            gGlobalSpacePrm.m_renderFrame++;
        }
    }
    gGlobalSpacePrm.m_interpolation += 0.1f;

//    end = std::chrono::system_clock::now();
//    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count(); //処理に要した時間をミリ秒に変換
//    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "fps=%f elapsed=%f[ms] m_xAngle=%f m_yAngle=%f. %s %s(%d)", 1000.0f/elapsed, elapsed, m_xAngle, m_yAngle, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
}

void Renderer::SetRotationAngles(float x, float y)
{
    gGlobalSpacePrm.m_rotationX = x;
    gGlobalSpacePrm.m_rotationY = y;
}

void Renderer::SetScale(float scale)
{
    gGlobalSpacePrm.m_scale = scale;
}
