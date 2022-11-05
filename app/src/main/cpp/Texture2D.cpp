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
	glDeleteTextures(1, &mTexture);
}

std::tuple<bool, int, int, unsigned char*> Texture2D::LoadTexture(const string &fileName, bool generateMipMaps)
{
	int width;
	int height;
	int components;

	// Use stbi image library to load our image
	unsigned char *rgbbindata = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);
	return {true, width, height, rgbbindata};
}

bool Texture2D::InitTexture(int w, int h, unsigned char *rgbabindata)
{
	if (rgbabindata == nullptr)
	{
		__android_log_print(ANDROID_LOG_INFO, "aaaaa", "Error loading texture %s(%d)", __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
		return false;
	}

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
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
	return true;
}

void Texture2D::Bind(GLuint texUnit) const
{
	assert(texUnit >= 0 && texUnit < 32);
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}
