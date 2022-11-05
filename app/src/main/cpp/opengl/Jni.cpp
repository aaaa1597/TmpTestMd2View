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

extern "C"
JNIEXPORT void JNICALL
Java_com_tks_cppmd2viewer_Jni_00024Companion_setRotate(JNIEnv *env, jobject thiz, jfloat x, jfloat y) {
    pRenderer->SetRotationAngles(x, y);
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
Java_com_tks_cppmd2viewer_Jni_00024Companion_setScale(JNIEnv *env, jobject thiz, jfloat scale) {
    pRenderer->SetScale(scale);
}
