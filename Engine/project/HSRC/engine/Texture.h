#pragma once

#include <FreeImage.h>
#include "ResourceManager.h"

#define DEFAULT_TEXTURE Mesh::getMesh("Textures/default.png")

class Texture : Resource
{
public:
	static Texture* getTexture(string texturePath);
	~Texture();
	//GLuint id();
private:
	Texture(string texturePath);
	//GLuint texID;
};
