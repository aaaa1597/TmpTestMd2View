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

GLuint Texture2D::InitTexture(int w, int h, unsigned char *rgbabindata)
{
	if (rgbabindata == nullptr)
	{
		__android_log_print(ANDROID_LOG_INFO, "aaaaa", "Error loading texture %s(%d)", __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
		return false;
	}

	GLuint retTexid;
	glGenTextures(1, &retTexid);
	glBindTexture(GL_TEXTURE_2D, retTexid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbabindata);

	if (/*wkGenerateMipMaps*/true)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// unbinding our texture
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(rgbabindata);
	return retTexid;
}

void Texture2D::Bind(GLuint texUnit, GLuint texture) const
{
	assert(texUnit >= 0 && texUnit < 32);
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
}
