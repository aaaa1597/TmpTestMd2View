#include <jni.h>
#include <android/log.h>
#include "Md2Parts.h"
#include "Md2Obj.h"
#include "TexObj.h"
#include "ShaderProgram.h"
#include "Texture2D.h"

static const std::string BASE_PATH = "/data/user/0/com.tks.cppmd2viewer/files/";

void Md2Model::setFileName(const char *md2FileName, const char *textureFileName) {
	__android_log_print(ANDROID_LOG_INFO, "aaaaa", "name(%s) %s %s(%d)", md2FileName, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
	auto [retbool, w, h, rgbabindata] = LoadTexture(BASE_PATH + std::string(textureFileName));
	InitTexture(w, h, rgbabindata);
	m_shaderProgram.LoadShaders(BASE_PATH  + "basic.vert", BASE_PATH + "basic.frag");
	InitBuffer();
}

void Md2Model::SetPosition(float x, float y, float z)
{
    m_position = glm::vec3(x,y,z);
}

Md2Model::~Md2Model()
{
	for (size_t i = 0; i < m_vboIndices.size(); i++)
	{
		glDeleteBuffers(1, &m_vboIndices[i]);
	}
}

void Md2Model::Draw(size_t frame, float xAngle, float yAngle, float scale, float interpolation, const glm::mat4 &view, const glm::mat4 &projection)
{
	glEnable(GL_DEPTH_TEST);
	assert(m_textureLoaded && m_bufferInitialized);
	m_texture.Bind(0);
	glm::mat4 model;

	model = glm::translate(model, m_position) *
	        glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f)) *
	        glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f)) *
	        glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
	        glm::scale(model, glm::vec3(0.3 * scale, 0.3 * scale, 0.3 * scale));

	m_shaderProgram.Use();
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
    m_shaderProgram.SetUniform("mvpmat", vpmat * model);
	/* ↑これもOK ここまで */

	auto count = m_frameIndices[frame].second - m_frameIndices[frame].first + 1;
	m_shaderProgram.SetUniform("interpolation", interpolation);

	glBindBuffer(GL_ARRAY_BUFFER, mVboId);
	glVertexAttribPointer(mCurPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0));
	glVertexAttribPointer(mNextPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLES, m_frameIndices[frame].first, count);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

std::tuple<bool, int, int, unsigned char *> Md2Model::LoadTexture(std::string textureFileName)
{
	return m_texture.LoadTexture(textureFileName, true);
}

void Md2Model::InitTexture(int w, int h, unsigned char *rgbabindata) {
	m_texture.InitTexture(w, h, rgbabindata);
	m_textureLoaded = true;
}

size_t Md2Model::GetEndFrame()
{
	return mMdlData.numTotalFrames - 1;
}

void Md2Model::InitBuffer()
{
	mCurPosAttrib = glGetAttribLocation(m_shaderProgram.GetProgram(), "pos");
	mNextPosAttrib = glGetAttribLocation(m_shaderProgram.GetProgram(), "nextPos");
	mTexCoordAttrib = glGetAttribLocation(m_shaderProgram.GetProgram(), "texCoord");

	std::vector<float> md2Vertices;
	const size_t startFrame = 0;
	size_t endFrame = GetEndFrame();
	vertex *currentFrame;
	vertex *nextFrame;
	mMdlData.currentFrame = startFrame;
	mMdlData.interpol = 0.0f;

	size_t vertexIndex = 0;
	size_t startVertex = 0;

    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "endFrame=%d numPoly=%d numVertexsperframe=%d %s %s(%d)", endFrame, mMdlData.numPolys, mMdlData.numVertexsPerFrame, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

	// fill buffer
	while (mMdlData.currentFrame <= endFrame)
	{
		currentFrame = &mMdlData.vertexList[mMdlData.numVertexsPerFrame * mMdlData.currentFrame];
		nextFrame = mMdlData.currentFrame == endFrame ? &mMdlData.vertexList[mMdlData.numVertexsPerFrame * startFrame] : &mMdlData.vertexList[mMdlData.numVertexsPerFrame * (mMdlData.currentFrame + 1)];
		startVertex = vertexIndex;
		for (size_t index = 0; index < mMdlData.numPolys; index++)
		{

			// Start of the vertex data
			for (size_t p = 0; p < 3; p++)
			{
				// current frame
				for (size_t j = 0; j < 3; j++)
				{
					// vertices
					md2Vertices.emplace_back(currentFrame[mMdlData.polyIndex[index].meshIndex[p]].v[j]);
				}

				// next frame
				for (size_t j = 0; j < 3; j++)
				{
					// vertices
					md2Vertices.emplace_back(nextFrame[mMdlData.polyIndex[index].meshIndex[p]].v[j]);
				}

				// tex coords
				md2Vertices.emplace_back(mMdlData.st[mMdlData.polyIndex[index].stIndex[p]].s);
				md2Vertices.emplace_back(mMdlData.st[mMdlData.polyIndex[index].stIndex[p]].t);
				vertexIndex++;
			}
			// End of the vertex data
		}

		m_frameIndices[mMdlData.currentFrame] = {startVertex, vertexIndex - 1};
		mMdlData.currentFrame++;
	}

	size_t frameIndex = startFrame;
	glGenBuffers(1, &mVboId); // Generate an empty vertex buffer on the GPU

	size_t count = m_frameIndices[frameIndex].second - m_frameIndices[frameIndex].first + 1;
	__android_log_print(ANDROID_LOG_INFO, "aaaaa", "count=%d (m_frameIndices[frameIndex].first * 8)=%d  %s %s(%d)", count, m_frameIndices[frameIndex].first * 8, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

	glBindBuffer(GL_ARRAY_BUFFER, mVboId);																											   // "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 8 * mMdlData.numTotalFrames, &md2Vertices[m_frameIndices[frameIndex].first * 8], GL_STATIC_DRAW); // copy the data from CPU to GPU

	// Current Frame Position attribute
	glVertexAttribPointer(mCurPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0));
	glEnableVertexAttribArray(mCurPosAttrib);

	// Next  Frame Position attribute
	glVertexAttribPointer(mNextPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(mNextPosAttrib);

	// Texture Coord attribute
	glVertexAttribPointer(mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(mTexCoordAttrib);
	m_vboIndices.emplace_back(mVboId);
	m_bufferInitialized = true;
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

bool Md2Model::LoadModel() {
	/* MD2ヘッダ */
	md2header *header = (md2header*)mWkMd2BinData.data();

	/* MD2形式チェック */
	if(header->magicnumber != MD2_IDENT) { /* "IDP2"じゃないとエラー */
		std::vector<char>().swap(mWkMd2BinData);
		const union { int i; char b[4]; } ngno = {header->magicnumber};
		__android_log_print(ANDROID_LOG_INFO, "aaaaa", "MD2フォーマット不正(magicnumber=%s) %s %s(%d)", ngno.b, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
		return false;
	}

	if(header->version != MD2_VERSION) { /* 8じゃないとエラー */
		std::vector<char>().swap(mWkMd2BinData);
		__android_log_print(ANDROID_LOG_INFO, "aaaaa", "MD2フォーマット不正(version=%d) %s %s(%d)", header->version, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
		return false;
	}

	/* 初期化 */
	mMdlData.numVertexsPerFrame= header->num_vertexs;
	mMdlData.numTotalFrames    = header->num_totalframes;

	/* 頂点読込み */
	mMdlData.vertexList.resize(header->num_vertexs * header->num_totalframes);
	__android_log_print(ANDROID_LOG_INFO, "aaaaa", "1frame当りの頂点数(%d)と総フレーム数(%d) %s %s(%d)", header->num_vertexs, header->num_totalframes, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
	for(size_t lpct = 0; lpct < header->num_totalframes; lpct++) {
		frame *lframe = (frame*)&(mWkMd2BinData[header->offset_frames + header->framesize * lpct]);
		vertex *pvertex = &mMdlData.vertexList[header->num_vertexs * lpct];
		for (size_t lpct2 = 0; lpct2 < header->num_vertexs; lpct2++) {
			pvertex[lpct2].v[0] = lframe->scale[0] * lframe->fp[lpct2].v[0] + lframe->translate[0];
			pvertex[lpct2].v[1] = lframe->scale[1] * lframe->fp[lpct2].v[1] + lframe->translate[1];
			pvertex[lpct2].v[2] = lframe->scale[2] * lframe->fp[lpct2].v[2] + lframe->translate[2];
		}
	}

	/* uvデータ読込み */
	mMdlData.st.resize(header->num_st);
	texindex *sts = (texindex*)&mWkMd2BinData[header->offset_st];
	for (size_t lpct = 0; lpct < header->num_st; lpct++) {
		mMdlData.st[lpct].s = static_cast<float>(sts[lpct].s) / static_cast<float>(header->skinwidth);
		mMdlData.st[lpct].t = static_cast<float>(sts[lpct].t) / static_cast<float>(header->skinheight);
	}

	/* mesh情報読込み */
	mMdlData.polyIndex.resize(header->num_polys);
	mMdlData.numPolys = header->num_polys;
	mesh *polyIndex = (mesh*)&mWkMd2BinData[header->offset_meshs];
	for (size_t lpct = 0; lpct < header->num_polys; lpct++) {
		mMdlData.polyIndex[lpct].meshIndex[0] = polyIndex[lpct].meshIndex[0];
		mMdlData.polyIndex[lpct].meshIndex[1] = polyIndex[lpct].meshIndex[1];
		mMdlData.polyIndex[lpct].meshIndex[2] = polyIndex[lpct].meshIndex[2];

		mMdlData.polyIndex[lpct].stIndex[0] = polyIndex[lpct].stIndex[0];
		mMdlData.polyIndex[lpct].stIndex[1] = polyIndex[lpct].stIndex[1];
		mMdlData.polyIndex[lpct].stIndex[2] = polyIndex[lpct].stIndex[2];
	}

	/* アニメ関連情報初期化 */
	mMdlData.currentFrame = 0;
	mMdlData.nextFrame = 1;
	mMdlData.interpol = 0.0;

	/* MD2バイナリデータ解放 */
	std::vector<char>().swap(mWkMd2BinData);

	return true;
}

bool Md2Model::LoadTexture() {
	auto [retbool, w, h, rgbabindata] = TexObj::LoadTexture(mWkTexBinData);
	if(retbool) {
		mWkWidth = w;
		mWkHeight= h;
		mWkRgbaData = rgbabindata;
	}
	return retbool;
}
