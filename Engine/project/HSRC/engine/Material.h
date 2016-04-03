#pragma once

#include "ResourceManager.h"
#include "SimpleShader.h"
#include "Texture.h"

#define DEFAULT_MATERIAL Material::getMaterial("DEFAULT")

class Material : Resource
{
public:
	static Material* getMaterial(string name);
	~Material();
	map<string, Texture*> textures;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
private:
	Material(string name);
};