#include <jni.h>
#include "Renderer.h"
#include <memory>

static std::unique_ptr<Raydelto::MD2Loader::Renderer> pRenderer = std::make_unique<Raydelto::MD2Loader::Renderer>();

extern "C" JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_onSurfaceCreated(JNIEnv *env, jobject thiz) {
    pRenderer->OnSurfaceCreated();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_onSurfaceChanged(JNIEnv *env, jobject thiz, jint width, jint height) {
    pRenderer->SetScreenSize(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_onDrawFrame(JNIEnv *env, jobject thiz) {
    pRenderer->OnDrawFrame();
}

extern "C" JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_MyRenderer_setScreenSize(
    JNIEnv *env,
    jobject /* this */,
    jint width,
    jint height)
{
    pRenderer->SetScreenSize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_MyRenderer_setRotationAngles(
    JNIEnv *env,
    jobject /* this */,
    jfloat x,
    jfloat y)
{
    pRenderer->SetRotationAngles(x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_MyRenderer_setScale(
    JNIEnv *env,
    jobject /* this */,
    jfloat scale)
{
    pRenderer->SetScale(scale);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_setRotate(JNIEnv *env, jobject thiz,
                                                       jfloat m_touch_angle_x,
                                                       jfloat m_touch_angle_y) {
    // TODO: implement setRotate()
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_onStart(JNIEnv *env, jobject thiz, jobject assets,
                                                     jobjectArray modelnames,
                                                     jobjectArray md2filenames,
                                                     jobjectArray texfilenames,
                                                     jobjectArray vshfilenames,
                                                     jobjectArray fshfilenames) {
    return true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_onStop(JNIEnv *env, jobject thiz) {
    // TODO: implement onStop()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_setModelPosition(JNIEnv *env, jobject thiz,
                                                              jstring modelname, jfloat x, jfloat y,
                                                              jfloat z) {
    // TODO: implement setModelPosition()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_setScale(JNIEnv *env, jobject thiz, jfloat m_scale) {
    // TODO: implement setScale()
}