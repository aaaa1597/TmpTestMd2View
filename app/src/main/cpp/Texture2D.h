#pragma once
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <string>

class Texture2D
{
public:
	Texture2D();
	virtual ~Texture2D();
	bool LoadTexture(const std::string &fileName, bool generateMipMaps = true);
	bool InitTexture();
	void Bind(GLuint texUnit = 0) const;

private:
	GLuint mTexture = 0;
	int wkWidth;
	int wkHeight;
	bool wkGenerateMipMaps;
	unsigned char *wkImageData = nullptr;
};
