#include <iostream>
#include <cassert>
#include <android/log.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "Texture2D.h"

using namespace Raydelto::MD2Loader;
using std::string;

Texture2D::Texture2D()
	: mTexture(0)
{
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mTexture);
}

bool Texture2D::LoadTexture(const string &fileName, bool generateMipMaps)
{
	int width;
	int height;
	int components;

	// Use stbi image library to load our image
	unsigned char *imageData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);

	if (imageData == nullptr)
	{
		__android_log_print(ANDROID_LOG_INFO, "aaaaa", "Error loading texture %s %s(%d)", fileName.c_str(), __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
		return false;
	}

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	if (generateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(imageData);
	// unbinding our texture
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

void Texture2D::Bind(GLuint texUnit) const
{
	assert(texUnit >= 0 && texUnit < 32);
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}
