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

/* OpenGLのTexture初期化 */
std::tuple<bool, GLuint>  GlObj::InitTexture(int width, int height, const char *rgbabindbuf) {
    bool generateMipMaps = true;    /* ひとまず、true固定 */

    GLuint texid;
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (generateMipMaps) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbabindbuf);

    if (generateMipMaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    /* 後片付け(unbinding our texture) */
    glBindTexture(GL_TEXTURE_2D, 0);

    return {true, texid};
}

std::tuple<bool, GLuint> GlObj::LoadShaders(const std::string &vshstrdata, const std::string &fshstrdata) {
    const GLchar *vsSourcePtr = vshstrdata.c_str();
    const GLchar *fsSourcePtr = fshstrdata.c_str();

    GLuint vsid = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsid = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vsid, 1, &vsSourcePtr, nullptr);
    glShaderSource(fsid, 1, &fsSourcePtr, nullptr);

    glCompileShader(vsid);
    bool ret0 = CheckCompileErrors(vsid, EShaderType::VERTEX_SHADER);
    if( !ret0) {
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Failed to compile vertex shader.");
        glDeleteShader(vsid);
        return {false, -1};
    }

    glCompileShader(fsid);
    bool ret1 = CheckCompileErrors(fsid, EShaderType::FRAGMENT_SHADER);
    if( !ret1) {
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Failed to compile fragment shader.");
        glDeleteShader(vsid);
        glDeleteShader(fsid);
        return {false, -1};
    }

    GLuint retProgramId = glCreateProgram();
    if (retProgramId == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Failed to create shader program!");
        return {false, -1};
    }

    /* Attach shader to program. */
    glAttachShader(retProgramId, vsid);
    glAttachShader(retProgramId, fsid);

    glLinkProgram(retProgramId);
    bool ret2 = CheckLinkError(retProgramId);
    if ( !ret2) {
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Failed!! Shader program failed to link.");
        glDetachShader(retProgramId, vsid);
        glDetachShader(retProgramId, fsid);
        glDeleteShader(vsid);
        glDeleteShader(fsid);
        glDeleteProgram(retProgramId);
        return {false, -1};
    }

    /* 準備完了 */
    glDeleteShader(vsid);
    glDeleteShader(fsid);

    return {true, retProgramId};
}

void GlObj::DeleteShaders(GLuint programId) {
    glDeleteProgram(programId);
}

/* コンパイル結果判定 */
bool GlObj::CheckCompileErrors(GLuint id, EShaderType type) {
    int isCompiled = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);

    /* 正常終了 */
    if (isCompiled != GL_FALSE)
        return true;

    /* 失敗 エラー文字列長取得 */
    int maxLength = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
    /* 格納領域確保 */
    char *gllogstr = (char*)malloc(maxLength);
    /* 結果文字列取得 */
    glGetShaderInfoLog(id, maxLength, &maxLength, gllogstr);
    if (type == EShaderType::VERTEX_SHADER) {
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "VertexShader : %s\n", gllogstr);
    }
    else {
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "FragmentShader : %s\n", gllogstr);
    }
    /* 格納領域解放 */
    free(gllogstr);

    return false;
}

bool GlObj::CheckLinkError(GLuint programId) {
    int status;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    if(status != 0)
        return true;

    int logLength;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        char *logstr = (char*)malloc(logLength);
        glGetProgramInfoLog(programId, logLength, &logLength, logstr);
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Program link Error!! : \n%s", logstr);
        free(logstr);
    }

    return false;
}

/* シェーダのAttribute属性一括設定 */
RetShaderAttribs GlObj::setAttribute(GLuint programId, int totalframes,
                                     const std::vector<vertex> &vertexs, const std::vector<mesh> &polyIndexs,const std::vector<texstcoord> &sts) {
    /* 返却値 */
    std::unordered_map<int, std::pair<int, int>> retAnimFrameS2e;
    GLuint retVboId = -1;
    GLuint retCurPosAttrib   = glGetAttribLocation( programId, "pos");
    GLuint retNextPosAttrib  = glGetAttribLocation( programId, "nextPos");
    GLuint retTexCoordAttrib = glGetAttribLocation( programId, "texCoord");

    /* 初期知設定 */
    std::vector<float> wkMd2Vertices = {0};
    const size_t numPolys           = polyIndexs.size();
    const size_t numVertexsperframe = vertexs.size() / totalframes;

    /* 現在頂点,次頂点,UV座標で詰替え */
    for(int frameidx = 0; frameidx < totalframes; frameidx++) {
        /* 現在フレームと次フレームを取得 */
        const vertex *currentFrame= &vertexs[numVertexsperframe * frameidx];
        const vertex *nextFrame   = (frameidx+1 >= totalframes) ? &vertexs[0] : &vertexs[numVertexsperframe * (frameidx+1)];

        for(int plyidx = 0; plyidx < numPolys; plyidx++) {
            for(int meshdx = 0; meshdx < 3; meshdx++) {
                /* vertices */
                for (size_t vidx = 0; vidx < 3; vidx++) {
                    wkMd2Vertices.emplace_back(currentFrame[polyIndexs[plyidx].meshIndex[meshdx]].v[vidx]);
                }

                /* next frame */
                for (size_t vidx = 0; vidx < 3; vidx++) {
                    // vertices
                    wkMd2Vertices.emplace_back(nextFrame[polyIndexs[plyidx].meshIndex[meshdx]].v[vidx]);
                }

                /* tex coords */
                wkMd2Vertices.emplace_back(sts[polyIndexs[plyidx].stIndex[meshdx]].s);
                wkMd2Vertices.emplace_back(sts[polyIndexs[plyidx].stIndex[meshdx]].t);
            }
        }

        int startverindex= (frameidx==0) ? 0 : retAnimFrameS2e[frameidx-1].second + 1;
        int endverindex  = ((frameidx+1) * numPolys * 3) - 1;
        retAnimFrameS2e[frameidx] = {startverindex, endverindex};
    }

    size_t numVertexs = numPolys * 3 + 1;
    glGenBuffers(1, &retVboId);

    glBindBuffer(GL_ARRAY_BUFFER, retVboId);
    glBufferData(GL_ARRAY_BUFFER, numVertexs * sizeof(float) * 8 * totalframes, &wkMd2Vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(retCurPosAttrib  , 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)nullptr);
    glVertexAttribPointer(retNextPosAttrib , 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(retTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));

    glEnableVertexAttribArray(retCurPosAttrib);
    glEnableVertexAttribArray(retNextPosAttrib);
    glEnableVertexAttribArray(retTexCoordAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return {true, retAnimFrameS2e, retVboId, retCurPosAttrib, retNextPosAttrib, retTexCoordAttrib};
}

void GlObj::activeTexture(GLenum texture) {
    glActiveTexture(texture);
}

void GlObj::bindTexture(GLenum target, GLuint textureid) {
    glBindTexture(target, textureid);
}

void GlObj::deleteTextures(GLsizei size, const GLuint *textures) {
    glDeleteTextures(size, textures);
}

void GlObj::useProgram(GLuint progId) {
    glUseProgram(progId);
}

void GlObj::deleteProgram(GLuint progid) {
    glDeleteProgram(progid);
}
