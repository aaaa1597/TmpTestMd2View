#include <jni.h>
#include <memory>
#include <map>
#include <mutex>
#include <chrono>
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include "Md2Obj.h"
#include "Renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

std::map<std::string, Md2Model>       gMd2Models;     /* Md2モデルデータ実体 */
std::mutex                            gMutex;         /* onStart()完了待ちmutex */
std::chrono::system_clock::time_point gPreStartTime;/* 前回開始時刻 */

static std::unique_ptr<Raydelto::MD2Loader::Renderer> pRenderer = std::make_unique<Raydelto::MD2Loader::Renderer>();

/* onStart */
JNIEXPORT jboolean JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_onStart(JNIEnv *env, jobject thiz, jobject assets,
                                                     jobjectArray modelnames, jobjectArray md2filenames, jobjectArray texfilenames,
                                                     jobjectArray vshfilenames, jobjectArray fshfilenames) {
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "%s %s(%d)", __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
    gMutex.lock();

    /* 引数チェック(md2model) */
    jsize size0 = env->GetArrayLength(modelnames), size1 = env->GetArrayLength(md2filenames), size2 = env->GetArrayLength(texfilenames), size3 = env->GetArrayLength(vshfilenames), size4 = env->GetArrayLength(fshfilenames);
    if(size0 != size1 || size0 != size2 || size0 != size3 || size0 != size4) {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "引数不正 名称リストの数が合わない!!! modelname.size=%d md2filenames.size=%d texfilenames.size=%d vshfilenames.size=%d fshfilenames.size=%d %s %s(%d)", size0, size1, size2, size3, size4, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
        return false;
    }

    /* AAssetManager取得 */
    AAssetManager *assetMgr = AAssetManager_fromJava(env, assets);
    if (assetMgr == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "ERROR loading asset!!");
        return false;
    }

    /* モデル名,頂点ファイル名,Texファイル名,頂点ファイル中身,Texファイル中身,vshファイルの中身,fshのファイルの中身 取得 */
    for(int lpct = 0; lpct < size0; lpct++) {
        /* jobjectArray -> jstring */
        jstring modelnamejstr   = (jstring)env->GetObjectArrayElement(modelnames  , lpct);
        jstring md2filenamejstr = (jstring)env->GetObjectArrayElement(md2filenames, lpct);
        jstring texfilenamejstr = (jstring)env->GetObjectArrayElement(texfilenames, lpct);
        jstring vshfilenamejstr = (jstring)env->GetObjectArrayElement(vshfilenames, lpct);
        jstring fshfilenamejstr = (jstring)env->GetObjectArrayElement(fshfilenames, lpct);

        /* jstring -> char */
        const char *modelnamechar   = env->GetStringUTFChars(modelnamejstr  , nullptr);
        const char *md2filenamechar = env->GetStringUTFChars(md2filenamejstr, nullptr);
        const char *texfilenamechar = env->GetStringUTFChars(texfilenamejstr, nullptr);
        const char *vshfilenamechar = env->GetStringUTFChars(vshfilenamejstr, nullptr);
        const char *fshfilenamechar = env->GetStringUTFChars(fshfilenamejstr, nullptr);

        /* md2関連データ一括読込み */
        std::vector<std::pair<std::string, std::vector<char>>> wk = {{md2filenamechar, std::vector<char>()},
                                                                     {texfilenamechar, std::vector<char>()},
                                                                     {vshfilenamechar, std::vector<char>()},
                                                                     {fshfilenamechar, std::vector<char>()}};
        for(std::pair<std::string, std::vector<char>> &item : wk) {
            const std::string &filename = item.first;
            std::vector<char> &binbuf = item.second;

            /* AAsset::open */
            AAsset *assetFile = AAssetManager_open(assetMgr, filename.c_str(), AASSET_MODE_RANDOM);
            if (assetFile == nullptr) {
                __android_log_print(ANDROID_LOG_INFO, "aaaaa", "ERROR AAssetManager_open(%s) %s %s(%d)", filename.c_str(), __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
                return false;
            }
            /* 読込 */
            int size = AAsset_getLength(assetFile);
            binbuf.resize(size);
            AAsset_read(assetFile, binbuf.data(), size);
            /* AAsset::close */
            AAsset_close(assetFile);
        }

        /* Md2model追加 */
        gMd2Models.emplace(modelnamechar, Md2Model{ .mName=modelnamechar,
                                                    .mWkMd2BinData=std::move(wk[0].second),
                                                    .mWkTexBinData=std::move(wk[1].second),
                                                    /* shaderはデータを文字列に変換して格納 */
                                                    .mWkVshStrData=std::string(wk[2].second.begin(), wk[2].second.end()),
                                                    .mWkFshStrData=std::string(wk[3].second.begin(), wk[3].second.end())});

        /* char解放 */
        env->ReleaseStringUTFChars(modelnamejstr  , modelnamechar);
        env->ReleaseStringUTFChars(md2filenamejstr, md2filenamechar);
        env->ReleaseStringUTFChars(texfilenamejstr, texfilenamechar);
        env->ReleaseStringUTFChars(vshfilenamejstr, vshfilenamechar);
        env->ReleaseStringUTFChars(fshfilenamejstr, fshfilenamechar);

        /* jstring解放 */
        env->DeleteLocalRef(modelnamejstr);
        env->DeleteLocalRef(md2filenamejstr);
        env->DeleteLocalRef(texfilenamejstr);
        env->DeleteLocalRef(vshfilenamejstr);
        env->DeleteLocalRef(fshfilenamejstr);
    }

    /* 初期化 */
    bool ret = pRenderer->LoadModel(gMd2Models);
    if(!ret) {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "Md2Obj::LoadModel()で失敗!! %s %s(%d)", __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
        return false;
    }

    gMutex.unlock();
    return true;
}

/* onSurfaceCreated */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_onSurfaceCreated(JNIEnv *env, jobject thiz) {
    pRenderer->OnSurfaceCreated(gMd2Models);
}

/* onSurfaceChanged */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_onSurfaceChanged(JNIEnv *env, jobject thiz, jint width, jint height) {
    pRenderer->OnSurfaceChanged(width, height);
}

/* onDrawFrame */
JNIEXPORT void JNICALL Java_com_tks_cppmd2viewer_Jni_00024Companion_onDrawFrame(JNIEnv *env, jobject thiz) {
    pRenderer->OnDrawFrame(gMd2Models);
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
