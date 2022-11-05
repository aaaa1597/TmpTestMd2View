#include <jni.h>
#include <memory>
#include <mutex>
#include <chrono>
#include "Renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

std::mutex                          gMutex;         /* onStart()完了待ちmutex */
std::chrono::system_clock::time_point gPreStartTime;/* 前回開始時刻 */

static std::unique_ptr<Raydelto::MD2Loader::Renderer> pRenderer = std::make_unique<Raydelto::MD2Loader::Renderer>();

/* onStart */
JNIEXPORT jboolean JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_onStart(JNIEnv *env, jobject thiz, jobject assets,
                                                     jobjectArray modelnames, jobjectArray md2filenames, jobjectArray texfilenames,
                                                     jobjectArray vshfilenames, jobjectArray fshfilenames) {
    return true;
}

/* onSurfaceCreated */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_onSurfaceCreated(JNIEnv *env, jobject thiz) {
    pRenderer->OnSurfaceCreated();
}

/* onSurfaceChanged */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_onSurfaceChanged(JNIEnv *env, jobject thiz, jint width, jint height) {
    pRenderer->OnSurfaceChanged(width, height);
}

/* onDrawFrame */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_onDrawFrame(JNIEnv *env, jobject thiz) {
    pRenderer->OnDrawFrame();
}

JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_onStop(JNIEnv *env, jobject thiz) {
    // TODO: implement onStop()
}

/* モデルデータ位置設定 */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_setModelPosition(JNIEnv *env, jobject thiz, jstring modelname, jfloat x, jfloat y, jfloat z) {
    // TODO: implement setModelPosition()
}

/* モデルデータ拡縮設定 */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_setScale(JNIEnv *env, jobject thiz, jfloat scale) {
    pRenderer->SetScale(scale);
}

/* モデルデータ回転設定 */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_setRotate(JNIEnv *env, jobject thiz, jfloat x, jfloat y) {
    pRenderer->SetRotationAngles(x, y);
}

#ifdef __cplusplus
};
#endif
