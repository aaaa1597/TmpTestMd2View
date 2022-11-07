#ifndef CPPMD2VIEWER_MD2OBJ_H
#define CPPMD2VIEWER_MD2OBJ_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Md2Parts.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define MD2_IDENT   (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')    /* magic number "IDP2" or 844121161 */
#define	MD2_VERSION 8                                           /* model version */

class ShaderProgram;

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

using RetShaderAttribs2 = std::tuple<bool, std::unordered_map<int, std::pair<int, int>>, GLuint, GLuint, GLuint, GLuint>;

class Md2Model {
public:
    ~Md2Model();
    // The frame parameter start at 0
    void Draw(const glm::mat4 &view, const glm::mat4 &projection);
    void SetPosition(float x, float y, float z);

public:
    bool LoadModel();
    bool LoadTexture(); /* AssetsからTextureデータを読込む */
    bool InitTexture(); /* TextureデータをOpenGLで使えるようにする */
    bool InitShaders(); /* シェーダをOpenGLで使えるようにする */

public:
    ShaderProgram       m_shaderProgram = {};

public:
    void setRotate(float x, float y);
    void setScale(float scale);

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
    glm::vec3               mPosition = glm::vec3(0.0f, 0.0f, -25.0f);
    /* アニメ関連 */
    std::unordered_map<int, std::pair<int, int>> mFrameIndices = {};
    int   mCurrentFrame = 0;
    float minterpolate = 0.0f;
    /* テクスチャ関連 */
    GLuint mTexId = -1;
    bool m_textureLoaded = false;
    /* シェーダー関連 */
    GLuint mVboId         = -1;
    GLuint mProgramId     = -1;
    GLuint mCurPosAttrib  = -1;
    GLuint mNextPosAttrib = -1;
    GLuint mTexCoordAttrib= -1;
    /* 姿勢制御 */
    float mScale  = 1.0f;
    float mRotatex= 180.0f;
    float mRotatey= 0;
    glm::mat4 m_model;
    glm::mat4 mvpmat;
};

class Md2Obj {
public:
    static bool LoadModel(std::map<std::string, Md2Model> &md2models);
    static bool InitModel(std::map<std::string, Md2Model> &md2models);
    using ArgType = std::tuple<const std::array<float, 16> &, const std::array<float, 16> &>;
    static bool DrawModel(std::map<std::string, Md2Model> &md2models, const ArgType &globalSpacePrm, float elapsedtimeMs);
    static void setScale(std::map<std::string, Md2Model> &md2models, float scale);
    static void setRotate(std::map<std::string, Md2Model> &md2models, float x, float y);
};

#endif //CPPMD2VIEWER_MD2OBJ_H
