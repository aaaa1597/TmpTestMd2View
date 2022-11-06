#pragma once

#include <string>
#include <tuple>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class Texture2D
{
public:
	Texture2D();
	virtual ~Texture2D();
	bool InitTexture(int w, int h, unsigned char *rgbabindata);
	void Bind(GLuint texUnit = 0) const;

private:
	GLuint mTexture = 0;
};
