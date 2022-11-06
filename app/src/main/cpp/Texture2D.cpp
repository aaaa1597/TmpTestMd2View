#include <iostream>
#include <cassert>
#include <android/log.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "Texture2D.h"

using std::string;

Texture2D::Texture2D()
{
}

Texture2D::~Texture2D()
{
//	glDeleteTextures(1, &mTexture);
}

void Texture2D::Bind(GLuint texUnit, GLuint texture) const
{
	assert(texUnit >= 0 && texUnit < 32);
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
}
