#include <vector>
#include <unordered_map>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Md2Parts.h"
#include "GlObj.h"

std::map<std::string, GLint> GlObj::mUniformLocations = {};

void GlObj::GlInit() {
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","GL_VERSION                  : %s\n", glGetString(GL_VERSION));
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","GL_VENDOR                   : %s\n", glGetString(GL_VENDOR));
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","GL_RENDERER                 : %s\n", glGetString(GL_RENDERER));
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","GL_SHADING_LANGUAGE_VERSION : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","GL_EXTENSIONS               : %s\n", glGetString(GL_EXTENSIONS));

    int value;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","gl_MaxVertexAttribs             (%4d) : %4d\n", 16, value);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &value);
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","gl_MaxVertexUniformVectors      (%4d) : %4d\n", 0, value);
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &value);
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","gl_MaxVaryingVectors            (%4d) : %4d\n", 0, value);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","gl_MaxCombinedTextureImageUnits (%4d) : %4d\n", 2, value);
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &value);
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","gl_MaxVertexTextureImageUnits   (%4d) : %4d\n", 0, value);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &value);
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","gl_MaxTextureImageUnits         (%4d) : %4d\n", 2, value);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &value);
    __android_log_print(ANDROID_LOG_INFO,"aaaaa","gl_MaxFragmentUniformVectors    (%4d) : %4d\n", 0, value);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    return;
}

void GlObj::activeTexture(GLenum texture) {
    glActiveTexture(texture);
}

void GlObj::bindTexture(GLenum target, GLuint textureid) {
    glBindTexture(target, textureid);
}
