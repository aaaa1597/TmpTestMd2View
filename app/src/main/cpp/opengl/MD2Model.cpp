#include "MD2Model.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include <jni.h>
#include <android/log.h>

using namespace Raydelto::MD2Loader;

static const std::string BASE_PATH = "/data/user/0/com.tks.cppmd2viewer/files/";


MD2Model::MD2Model(const char *md2FileName, const char *textureFileName) : m_texture(std::make_unique<Texture2D>()),
																		   m_shaderProgram(std::make_unique<ShaderProgram>()),
																		   m_position(glm::vec3(0.0f, 0.0f, -25.0f)),
																		   m_modelLoaded(false),
																		   m_textureLoaded(false),
																		   m_bufferInitialized(false),
																		   m_posAttrib(NULL),
																		   m_nextPosAttrib(NULL),
																		   m_texCoordAttrib(NULL)
{
	LoadTexture(BASE_PATH + std::string(textureFileName));
	LoadModel(BASE_PATH + std::string(md2FileName));
	m_shaderProgram->LoadShaders(BASE_PATH  + "basic.vert", BASE_PATH + "basic.frag");
	InitBuffer();
}


void MD2Model::SetPosition(float x, float y, float z)
{
    m_position = glm::vec3(x,y,z);
}

MD2Model::~MD2Model()
{
	for (size_t i = 0; i < m_vboIndices.size(); i++)
	{
		glDeleteBuffers(1, &m_vboIndices[i]);
	}
}

void MD2Model::Draw(size_t frame, float xAngle, float yAngle, float scale, float interpolation, const glm::mat4 &view, const glm::mat4 &projection)
{
	glEnable(GL_DEPTH_TEST);
	assert(m_modelLoaded && m_textureLoaded && m_bufferInitialized);
	m_texture->Bind(0);
	glm::mat4 model;

	model = glm::translate(model, m_position) *
	        glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f)) *
	        glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f)) *
	        glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
	        glm::scale(model, glm::vec3(0.3 * scale, 0.3 * scale, 0.3 * scale));

	m_shaderProgram->Use();
//	m_shaderProgram->SetUniform("model", model);
//	m_shaderProgram->SetUniform("view", view);
//	m_shaderProgram->SetUniform("projection", projection);
//	m_shaderProgram->SetUniform("modelView", view * model);

	/* ↓これOK ここから */
//    const glm::mat4 &vmmat = view * model;
//    m_shaderProgram->SetUniform("mvpmat", projection * vmmat);
	/* ↑これOK ここまで */
	/* ↓これもOK ここから */
    const glm::mat4 &vpmat = projection * view;
    m_shaderProgram->SetUniform("mvpmat", vpmat * model);
	/* ↑これもOK ここまで */

	auto count = m_frameIndices[frame].second - m_frameIndices[frame].first + 1;
	m_shaderProgram->SetUniform("interpolation", interpolation);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0));
	glVertexAttribPointer(m_nextPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(m_texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLES, m_frameIndices[frame].first, count);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void MD2Model::LoadTexture(std::string textureFileName)
{
	m_texture->LoadTexture(textureFileName, true);
	m_textureLoaded = true;
}

size_t MD2Model::GetEndFrame()
{
	return m_model->numFrames - 1;
}

void MD2Model::InitBuffer()
{
	m_posAttrib = glGetAttribLocation( m_shaderProgram->GetProgram(), "pos");
	m_nextPosAttrib = glGetAttribLocation( m_shaderProgram->GetProgram(), "nextPos");
	m_texCoordAttrib = glGetAttribLocation( m_shaderProgram->GetProgram(), "texCoord");

	std::vector<float> md2Vertices;
	const size_t startFrame = 0;
	size_t endFrame = GetEndFrame();
	Raydelto::MD2Loader::vertex *currentFrame;
	Raydelto::MD2Loader::vertex *nextFrame;
	m_model->currentFrame = startFrame;
	m_model->interpol = 0.0f;

	size_t vertexIndex = 0;
	size_t startVertex = 0;

    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "endFrame=%d numPoly=%d numVertexsperframe=%d %s %s(%d)", endFrame, m_model->numTriangles, m_model->numVertexs, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

	// fill buffer
	while (m_model->currentFrame <= endFrame)
	{
		currentFrame = &m_model->vertexList[m_model->numVertexs * m_model->currentFrame];
		nextFrame = m_model->currentFrame == endFrame ? &m_model->vertexList[m_model->numVertexs * startFrame] : &m_model->vertexList[m_model->numVertexs * (m_model->currentFrame + 1)];
		startVertex = vertexIndex;
		for (size_t index = 0; index < m_model->numTriangles; index++)
		{

			// Start of the vertex data
			for (size_t p = 0; p < 3; p++)
			{
				// current frame
				for (size_t j = 0; j < 3; j++)
				{
					// vertices
					md2Vertices.emplace_back(currentFrame[m_model->polyIndx[index].meshIndex[p]].v[j]);
				}

				// next frame
				for (size_t j = 0; j < 3; j++)
				{
					// vertices
					md2Vertices.emplace_back(nextFrame[m_model->polyIndx[index].meshIndex[p]].v[j]);
				}

				// tex coords
				md2Vertices.emplace_back(m_model->st[m_model->polyIndx[index].stIndex[p]].s);
				md2Vertices.emplace_back(m_model->st[m_model->polyIndx[index].stIndex[p]].t);
				vertexIndex++;
			}
			// End of the vertex data
		}

		m_frameIndices[m_model->currentFrame] = {startVertex, vertexIndex - 1};
		m_model->currentFrame++;
	}

	size_t frameIndex = startFrame;
	glGenBuffers(1, &m_vbo); // Generate an empty vertex buffer on the GPU

	size_t count = m_frameIndices[frameIndex].second - m_frameIndices[frameIndex].first + 1;
	__android_log_print(ANDROID_LOG_INFO, "aaaaa", "count=%d (m_frameIndices[frameIndex].first * 8)=%d  %s %s(%d)", count, m_frameIndices[frameIndex].first * 8, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);																											   // "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 8 * m_model->numFrames, &md2Vertices[m_frameIndices[frameIndex].first * 8], GL_STATIC_DRAW); // copy the data from CPU to GPU

	// Current Frame Position attribute
	glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0));
	glEnableVertexAttribArray(m_posAttrib);

	// Next  Frame Position attribute
	glVertexAttribPointer(m_nextPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(m_nextPosAttrib);

	// Texture Coord attribute
	glVertexAttribPointer(m_texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(m_texCoordAttrib);
	m_vboIndices.emplace_back(m_vbo);
	m_bufferInitialized = true;
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void MD2Model::LoadModel(std::string md2FileName)
{
	FILE *fp;
	size_t length;

	char *buffer;

	header *head;
	texindex *stPtrs;

	mesh *bufIndexPtr;

	fp = fopen(md2FileName.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buffer = reinterpret_cast<char*>(malloc(length + 1));
	fread(buffer, sizeof(char), length, fp);

	head = reinterpret_cast<header*>(buffer);
    /* TODO 削除予定 */
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "endFrame=%d %s %s(%d)", head->Number_Of_Frames, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

	m_model.reset(reinterpret_cast<modData*>(malloc(sizeof(modData))));

	m_model->vertexList = reinterpret_cast<Raydelto::MD2Loader::vertex*>(malloc(sizeof(Raydelto::MD2Loader::vertex) * head->vNum * head->Number_Of_Frames));
	m_model->numVertexs = head->vNum;
	m_model->numFrames = head->Number_Of_Frames;
	m_model->frameSize = head->framesize;

	for (size_t lpct = 0; lpct < head->Number_Of_Frames; lpct++)
	{
		frame *fra = (frame *)&buffer[head->offsetFrames + head->framesize * lpct];
		Raydelto::MD2Loader::vertex *vertexList = (Raydelto::MD2Loader::vertex *)&m_model->vertexList[head->vNum * lpct];
		for (size_t lpct2 = 0; lpct2 < head->vNum; lpct2++)
		{
			vertexList[lpct2].v[0] = fra->scale[0] * fra->fp[lpct2].v[0] + fra->translate[0];
			vertexList[lpct2].v[1] = fra->scale[1] * fra->fp[lpct2].v[1] + fra->translate[1];
			vertexList[lpct2].v[2] = fra->scale[2] * fra->fp[lpct2].v[2] + fra->translate[2];
		}
	}

	m_model->st = reinterpret_cast<textcoord*>(malloc(sizeof(textcoord) * head->num_st));
	m_model->numST = head->num_st;
	stPtrs = (texindex *)&buffer[head->offsetTCoord];

	for (size_t count = 0; count < head->num_st; count++)
	{
		m_model->st[count].s = static_cast<float>(stPtrs[count].s) / static_cast<float>(head->twidth);
		m_model->st[count].t = static_cast<float>(stPtrs[count].t) / static_cast<float>(head->theight);
	}

	m_model->polyIndx = reinterpret_cast<mesh*>(malloc(sizeof(mesh) * head->num_polys));
	m_model->numTriangles = head->num_polys;
	bufIndexPtr = (mesh *)&buffer[head->offsetMesh];

//	for (size_t count = 0; count < head->Number_Of_Frames; count++)
//	{
		for (size_t count2 = 0; count2 < head->num_polys; count2++)
		{
			m_model->polyIndx[count2].meshIndex[0] = bufIndexPtr[count2].meshIndex[0];
			m_model->polyIndx[count2].meshIndex[1] = bufIndexPtr[count2].meshIndex[1];
			m_model->polyIndx[count2].meshIndex[2] = bufIndexPtr[count2].meshIndex[2];

			m_model->polyIndx[count2].stIndex[0] = bufIndexPtr[count2].stIndex[0];
			m_model->polyIndx[count2].stIndex[1] = bufIndexPtr[count2].stIndex[1];
			m_model->polyIndx[count2].stIndex[2] = bufIndexPtr[count2].stIndex[2];
		}
//	}

	m_model->currentFrame = 0;
	m_model->nextFrame = 1;
	m_model->interpol = 0.0;

	fclose(fp);
	free(buffer);
	m_modelLoaded = true;
}
