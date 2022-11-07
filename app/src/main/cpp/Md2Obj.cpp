#include <jni.h>
#include <android/log.h>
#include "Md2Parts.h"
#include "Md2Obj.h"
#include "TexObj.h"
#include "GlObj.h"
#include "ShaderProgram.h"
#include "GlobalSpaceObj.h"

GlobalSpaceObj gGlobalSpacePrm;/* グローバル空間パラメータ */

/* Md2モデル読込み(model読込,tex読込) */
bool Md2Obj::LoadModel(std::map<std::string, Md2Model> &md2models) {
    __android_log_print(ANDROID_LOG_INFO, "aaaaa", "%s %s(%d)", __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

    for(auto &[key, value] : md2models) {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "Md2Model load start (%s). %s %s(%d)", value.mName.c_str(), __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
        /* MD2モデルLoad */
        bool ret = value.LoadModel();
        std::vector<char>().swap(value.mWkMd2BinData);
        if( !ret) return false;
        /* テクスチャLoad */
        bool ret1 = value.LoadTexture();
        std::vector<char>().swap(value.mWkTexBinData);
        if( !ret1) return false;
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "Md2Model and Texture LOADED(%s). %s %s(%d)", key.c_str(), __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
    }

    return true;
}

/* Md2モデル初期化(特にOpenGL系は、onSurfaceCreated()ドリブンで動作しないとエラーになる) */
bool Md2Obj::InitModel(std::map<std::string, Md2Model> &md2models) {
    for(auto &[key, value] : md2models) {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "Md2Model Init start (%s). %s %s(%d)", value.mName.c_str(), __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
        /* テクスチャInit */
        bool ret2 = value.InitTexture();
        std::vector<char>().swap(value.mWkTexBinData);
        if( !ret2) return false;
      /* シェーダ初期化 */
      bool ret3 = value.InitShaders();
      if( !ret3) return false;
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "Shader Init end(%s). %s %s(%d)", key.c_str(), __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
    }
    return true;
}

/* Md2モデル描画 */
bool Md2Obj::DrawModel(std::map<std::string, Md2Model> &md2models, const Md2Obj::ArgType &globalSpacePrm, float elapsedtimeMs) {
    const std::array<float, 16> &aMvpMat     = std::get<0>(globalSpacePrm);
    const std::array<float, 16> &amNormalMat = std::get<1>(globalSpacePrm);


	Md2Model *m_player = &md2models.at("female");
	Md2Model *m_player2= &md2models.at("grunt");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_player->Draw(gGlobalSpacePrm.m_view, gGlobalSpacePrm.m_projection);
	m_player2->Draw(gGlobalSpacePrm.m_view, gGlobalSpacePrm.m_projection);

/* glEnable(GL_DEPTH_TEST); */
//    GlObj::enable(GL_DEPTH_TEST);
//
//    for(auto &[key, value] : md2models) {
//        value.DrawModel(aMvpMat, amNormalMat, Scale, Rotatex, Rotatey, elapsedtimeMs);
//    }
    return true;
}

void Md2Obj::setRotate(std::map<std::string, Md2Model> &md2models, float x, float y) {
	for(auto &[key, value] : md2models) {
		value.setRotate(x, y);
	}
	return;
}

void Md2Obj::setScale(std::map<std::string, Md2Model> &md2models, float scale) {
	for(auto &[key, value] : md2models) {
		value.setScale(scale);
	}
}

void Md2Model::SetPosition(float x, float y, float z) {
	mPosition = glm::vec3(x, y, z);
}

Md2Model::~Md2Model()
{
	glDeleteBuffers(1, &mVboId);
}

void Md2Model::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
	glEnable(GL_DEPTH_TEST);
	assert(m_textureLoaded);

	/* TODO 移動予定 glActiveTexture() → glBindTexture() */
	GlObj::activeTexture(GL_TEXTURE0);
	GlObj::bindTexture(GL_TEXTURE_2D, mTexId);

	glm::mat4 model;

    m_model = glm::translate(model, mPosition) *
			glm::rotate(model, glm::radians(mRotatey), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(model, glm::radians(mRotatex), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::scale(model, glm::vec3(0.3 * mScale, 0.3 * mScale, 0.3 * mScale));

	GlObj::useProgram(mProgramId);
//	m_shaderProgram.Use();
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
    m_shaderProgram.SetUniform(mProgramId, "mvpmat", vpmat * m_model);
	/* ↑これもOK ここまで */

	auto count = mFrameIndices[mCurrentFrame].second - mFrameIndices[mCurrentFrame].first + 1;
	m_shaderProgram.SetUniform(mProgramId,"interpolation", minterpolate);

	glBindBuffer(GL_ARRAY_BUFFER, mVboId);
	glVertexAttribPointer(mCurPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0));
	glVertexAttribPointer(mNextPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLES, mFrameIndices[mCurrentFrame].first, count);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	if(minterpolate >= 1.0f) {
		minterpolate = 0.0f;
		if(mCurrentFrame >= (mFrameIndices.size()-1))
			mCurrentFrame = 0;
		else
			mCurrentFrame++;
	}
	minterpolate += 0.1f;
}

RetShaderAttribs2 Md2Model::InitBuffer(GLuint programId, int totalframes,
									   const std::vector<vertex> &vertexs, const std::vector<mesh> &polyIndexs,const std::vector<texstcoord> &sts) {
	/* 返却値 */
	std::unordered_map<int, std::pair<int, int>> retAnimFrameS2e;
	GLuint retVboId = -1;
	GLuint retCurPosAttrib  = glGetAttribLocation(programId, "pos");
	GLuint retNextPosAttrib = glGetAttribLocation(programId, "nextPos");
	GLuint retTexCoordAttrib= glGetAttribLocation(programId, "texCoord");

	/* 初期知設定 */
	std::vector<float> wkMd2Vertices = {};
	const size_t numPolys           = polyIndexs.size();
	const size_t numVertexsperframe = vertexs.size() / totalframes;

	mMdlData.interpol = 0.0f;

	// fill buffer
	for(int frameidx = 0; frameidx < totalframes; frameidx++) {
		const vertex *currentFrame= &vertexs[numVertexsperframe * frameidx];
		const vertex *nextFrame   = (frameidx+1 >= totalframes) ? &vertexs[0] : &vertexs[numVertexsperframe * (frameidx+1)];

		for (size_t plyidx = 0; plyidx < numPolys; plyidx++) {
			for (size_t meshidx = 0; meshidx < 3; meshidx++) {
				/* now frame */
				for (size_t vidx = 0; vidx < 3; vidx++) {
					wkMd2Vertices.emplace_back(currentFrame[polyIndexs[plyidx].meshIndex[meshidx]].v[vidx]);
				}

				/* next frame */
				for (size_t vidx = 0; vidx < 3; vidx++) {
					wkMd2Vertices.emplace_back(nextFrame[polyIndexs[plyidx].meshIndex[meshidx]].v[vidx]);
				}

				/* tex coords */
				wkMd2Vertices.emplace_back(mMdlData.st[polyIndexs[plyidx].stIndex[meshidx]].s);
				wkMd2Vertices.emplace_back(mMdlData.st[polyIndexs[plyidx].stIndex[meshidx]].t);
			}
		}

		int startverindex= (frameidx==0) ? 0 : retAnimFrameS2e[frameidx-1].second + 1;
		int endverindex  = ((frameidx+1) * numPolys * 3) - 1;
		retAnimFrameS2e[frameidx] = {startverindex, endverindex};
	}

	size_t numVertexs = numPolys * 3;
	glGenBuffers(1, &retVboId); // Generate an empty vertex buffer on the GPU

	glBindBuffer(GL_ARRAY_BUFFER, retVboId);																											   // "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, numVertexs * sizeof(float) * 8 * totalframes, &wkMd2Vertices[0], GL_STATIC_DRAW); // copy the data from CPU to GPU

	// Current Frame Position attribute
	glVertexAttribPointer(retCurPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0));
	glEnableVertexAttribArray(retCurPosAttrib);

	// Next  Frame Position attribute
	glVertexAttribPointer(retNextPosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(retNextPosAttrib);

	// Texture Coord attribute
	glVertexAttribPointer(retTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(retTexCoordAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	return {true, retAnimFrameS2e, retVboId, retCurPosAttrib, retNextPosAttrib, retTexCoordAttrib};
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

/* AssetsからTextureデータを読込む */
bool Md2Model::LoadTexture() {
	auto [retbool, w, h, rgbabindata] = TexObj::LoadTexture(mWkTexBinData);
	if(retbool) {
		mWkWidth = w;
		mWkHeight= h;
		mWkRgbaData = std::move(rgbabindata);
	}
	__android_log_print(ANDROID_LOG_INFO, "aaaaa", "w,h(%d,%d) %s %s(%d)", mWkWidth, mWkHeight, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
//	for(int lpct = 0; lpct < mWkRgbaData.size(); lpct+=16) {
//		__android_log_print(ANDROID_LOG_INFO, "aaaaa", "[%d](%x,%x,%x,%x,%x,%x,%x,%x %x,%x,%x,%x,%x,%x,%x,%x) %s %s(%d)", lpct,
//							mWkRgbaData[lpct+ 0],mWkRgbaData[lpct+ 1],mWkRgbaData[lpct+ 2],mWkRgbaData[lpct+ 3],
//							mWkRgbaData[lpct+ 4],mWkRgbaData[lpct+ 5],mWkRgbaData[lpct+ 6],mWkRgbaData[lpct+ 7],
//							mWkRgbaData[lpct+ 8],mWkRgbaData[lpct+ 9],mWkRgbaData[lpct+10],mWkRgbaData[lpct+11],
//							mWkRgbaData[lpct+12],mWkRgbaData[lpct+13],mWkRgbaData[lpct+14],mWkRgbaData[lpct+15],
//							__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
//	}
	return retbool;
}

/* TextureデータをOpenGLで使えるようにする */
bool Md2Model::InitTexture() {
    /* OpenGLのTexture初期化 */
    auto[boolret, texid] = GlObj::InitTexture(mWkWidth, mWkHeight, mWkRgbaData.data());
    if(boolret)
        mTexId = texid;

    /* 解放処理 */
    mWkWidth = 0;
    mWkHeight= 0;
	std::vector<char>().swap(mWkRgbaData);
	m_textureLoaded = true;

    return boolret;
}

/* シェーダをOpenGLで使えるようにする */
bool Md2Model::InitShaders() {
    /* シェーダ読込み */
    auto[retboot, progid] = GlObj::LoadShaders(mWkVshStrData, mWkFshStrData);
    if( !retboot) {
        mProgramId = -1;
        return false;
    }
    mProgramId = progid;

    /* シェーダのAttributeにデータ一括設定 */
    auto[retbool, retAnimFrameS2e, retVboID, retCurPosAttrib, retNextPosAttrib, retTexCoordAttrib] = GlObj::setAttribute(mProgramId, mMdlData.numTotalFrames, mMdlData.vertexList, mMdlData.polyIndex, mMdlData.st);
    if( !retbool) {
        GlObj::DeleteShaders(mProgramId);
        mProgramId =-1;
        return false;
    }

    mFrameIndices  = std::move(retAnimFrameS2e);
    mVboId         = retVboID;
    mCurPosAttrib  = retCurPosAttrib;
    mNextPosAttrib = retNextPosAttrib;
    mTexCoordAttrib= retTexCoordAttrib;

    return true;
}

void Md2Model::setRotate(float x, float y) {
	mRotatex += x;
	mRotatey -= y;
}

void Md2Model::setScale(float scale) {
	mScale = scale;
}

