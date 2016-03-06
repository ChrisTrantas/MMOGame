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
	SimpleVertexShader* getVertexShader();
	SimplePixelShader* getPixelShader();
	Texture* diffuse;
	Texture* normalMap;
private:
	Material(string name);
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
};