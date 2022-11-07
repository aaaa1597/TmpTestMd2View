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
