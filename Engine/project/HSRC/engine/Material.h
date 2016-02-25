#pragma once

#include "ResourceManager.h"
#include "SimpleShader.h"

#define DEFAULT_MATERIAL Material::getMaterial("DEFAULT")

class Material : Resource
{
public:
	static Material* getMaterial(string name);
	~Material();
	SimpleVertexShader* getVertexShader();
	SimplePixelShader* getPixelShader();
private:
	Material(string name);
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
};