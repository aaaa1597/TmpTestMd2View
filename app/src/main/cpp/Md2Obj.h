#ifndef CPPMD2VIEWER_MD2OBJ_H
#define CPPMD2VIEWER_MD2OBJ_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Md2Parts.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Texture2D.h"

#define MD2_IDENT   (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')    /* magic number "IDP2" or 844121161 */
#define	MD2_VERSION 8                                           /* model version */

class ShaderProgram;
class Texture2D;

struct MdlData {
    int numTotalFrames;
    int numVertexsPerFrame;
    int numPolys;
    int twidth;
    int theight;
    int currentFrame;
    int nextFrame;
    float interpol;
    std::vector<vertex>     vertexList;
    std::vector<texstcoord> st;
    std::vector<mesh>       polyIndex;
    float x, y, z;
    float nextX, nextY, nextZ;
    float radius;
    float dist_to_player;
    int state;
    float speed;
};

class Md2Model {
public:
//    Md2Model(const char *md2FileName, const char *textureFileName);
    void setFileName(const char *md2FileName, const char *textureFileName);
    ~Md2Model();
    // The frame parameter start at 0
    void Draw(size_t frame, float xAngle, float yAngle, float scale, float interpolation, const glm::mat4 &view, const glm::mat4 &projection);
    size_t GetEndFrame();
    void SetPosition(float x, float y, float z);

public:
    bool LoadModel();
    bool LoadTexture();

private:
    void LoadTexture(std::string textureFileName);
    void InitTexture();
    void InitBuffer();

public:
    ShaderProgram       m_shaderProgram = {};
    std::vector<GLuint> m_vboIndices = {};

public:
    std::string         mName = {0};
    std::vector<char>   mWkMd2BinData = {0};
    std::vector<char>   mWkTexBinData = {0};
    std::string         mWkVshStrData = {0};
    std::string         mWkFshStrData = {0};
    int                 mWkWidth = 0;
    int                 mWkHeight= 0;
    std::vector<char>   mWkRgbaData = {0};
    /* 描画に必要なデータ */
    MdlData                 mMdlData = {0};
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, -25.0f);
    /* アニメ関連 */
    std::unordered_map<int, std::pair<int, int>> m_frameIndices = {};
    /* テクスチャ関連 */
    Texture2D           m_texture = {};
    bool m_textureLoaded = false;
    bool m_bufferInitialized = false;
    /* シェーダー関連 */
    GLuint mVboId         = -1;
    GLuint mCurPosAttrib  = -1;
    GLuint mNextPosAttrib = -1;
    GLuint mTexCoordAttrib= -1;
};

class Md2Obj {
public:
    static bool LoadModel(std::map<std::string, Md2Model> &md2models);
    static bool InitModel(std::map<std::string, Md2Model> &md2models);
    using ArgType = std::tuple<const std::array<float, 16> &, const std::array<float, 16> &, float, float, float>;
    static bool DrawModel(std::map<std::string, Md2Model> &md2models, const ArgType &globalSpacePrm, float elapsedtimeMs);
};

#endif //CPPMD2VIEWER_MD2OBJ_H
