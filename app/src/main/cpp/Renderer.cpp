#include "Renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <android/log.h>
#include "GlobalSpaceObj.h"

GlobalSpaceObj                        gGlobalSpacePrm;/* グローバル空間パラメータ */

void Renderer::OnDrawFrame(std::map<std::string, Md2Model> &md2models)
{
//    std::chrono::system_clock::time_point  start, end; // 型は auto で可
//    start = std::chrono::system_clock::now(); // 計測開始時間

//    Md2Model *m_player = &md2models.at("female");
//    Md2Model *m_player2= &md2models.at("grunt");
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    m_player->Draw(gGlobalSpacePrm.mRotatex, gGlobalSpacePrm.mRotatey, gGlobalSpacePrm.mScale, gGlobalSpacePrm.m_view, gGlobalSpacePrm.m_projection, gGlobalSpacePrm.m_model);
//    m_player2->Draw(gGlobalSpacePrm.mRotatex, gGlobalSpacePrm.mRotatey, gGlobalSpacePrm.mScale, gGlobalSpacePrm.m_view, gGlobalSpacePrm.m_projection, gGlobalSpacePrm.m_model);

//    end = std::chrono::system_clock::now();
//    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count(); //処理に要した時間をミリ秒に変換
//    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "fps=%f elapsed=%f[ms] m_xAngle=%f m_yAngle=%f. %s %s(%d)", 1000.0f/elapsed, elapsed, m_xAngle, m_yAngle, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
}
