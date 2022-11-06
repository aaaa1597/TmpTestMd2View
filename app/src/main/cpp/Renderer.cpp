#include "Renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <android/log.h>

using namespace Raydelto::MD2Loader;

Renderer::Renderer() = default;

void Renderer::OnSurfaceChanged(int width, int height) {
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "(w,h)=(%d,%d) %s %s(%d)", width, height, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

    // Create the projection matrix
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    m_camPos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_targetPos = glm::vec3(0.0f, 0.0f, -20.0f);
    m_up = glm::vec3(1.0f, 0.0f, 0.0f);

    // Create the View matrix
    m_view = glm::lookAt(m_camPos, m_camPos + m_targetPos, m_up);
}

void Renderer::OnDrawFrame(std::map<std::string, Md2Model> &md2models)
{
//    std::chrono::system_clock::time_point  start, end; // 型は auto で可
//    start = std::chrono::system_clock::now(); // 計測開始時間

    Md2Model *m_player = &md2models.at("female");
    Md2Model *m_player2= &md2models.at("grunt");

    static const int START_FRAME = 0;
    static const int END_FRAME = m_player->GetEndFrame();

    m_xAngle += m_rotationX;
    m_yAngle -= m_rotationY;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_player->Draw(m_renderFrame, m_xAngle, m_yAngle, m_scale, m_interpolation, m_view, m_projection);
    m_player2->Draw(m_renderFrame, m_xAngle, m_yAngle, m_scale, m_interpolation, m_view, m_projection);
    if (m_interpolation >= 1.0f)
    {
        m_interpolation = 0.0f;
        if (m_renderFrame == END_FRAME)
        {
            m_renderFrame = START_FRAME;
        }
        else
        {
            m_renderFrame++;
        }
    }
    m_interpolation += 0.1f;

//    end = std::chrono::system_clock::now();
//    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count(); //処理に要した時間をミリ秒に変換
//    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "fps=%f elapsed=%f[ms] m_xAngle=%f m_yAngle=%f. %s %s(%d)", 1000.0f/elapsed, elapsed, m_xAngle, m_yAngle, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
}

void Renderer::SetRotationAngles(float x, float y)
{
    m_rotationX = x;
    m_rotationY = y;
}

void Renderer::SetScale(float scale)
{
    m_scale = scale;
}
