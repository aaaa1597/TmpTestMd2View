#pragma once

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

namespace Raydelto::MD2Loader
{
    class ShaderProgram;
    class Texture2D;

    struct MdlData
    {
        int numTotalFrames;
        int numVertexsPerFrame;
        int numPolys;
        int twidth;
        int theight;
        int currentFrame;
        int nextFrame;
        float interpol;
        vertex *vertexList;
        texstcoord *st;
        mesh *polyIndx;
        float x, y, z;
        float nextX, nextY, nextZ;
        float radius;
        float dist_to_player;
        int state;
        float speed;
    };

    class MD2Model
    {
    public:
        MD2Model(const char *md2FileName, const char *textureFileName);
        ~MD2Model();
        // The frame parameter start at 0
        void Draw(size_t frame, float xAngle, float yAngle, float scale, float interpolation, const glm::mat4 &view, const glm::mat4 &projection);
        size_t GetEndFrame();
        void SetPosition(float x, float y, float z);

    private:
        void LoadModel(std::string md2FileName);
        void LoadTexture(std::string textureFileName);
        void InitBuffer();

        std::unique_ptr<MdlData> m_model;
        std::unique_ptr<Texture2D> m_texture;
        std::vector<GLuint> m_vboIndices;
        std::unique_ptr<ShaderProgram> m_shaderProgram;
        std::unordered_map<int, std::pair<int, int>> m_frameIndices;
        glm::vec3 m_position;
        bool m_modelLoaded;
        bool m_textureLoaded;
        bool m_bufferInitialized;
        GLuint m_vbo;
        GLuint m_posAttrib;
        GLuint m_nextPosAttrib;
        GLuint m_texCoordAttrib;
    };
}
