#pragma once

#include "ResourceManager.h"
#include "SimpleShader.h"
#include "Texture.h"

#define DEFAULT_MATERIAL Material::getMaterial("DEFAULT")

#define VERTEX_BIT 0x1
#define PIXEL_BIT 0x2
#define DOMAIN_BIT 0x4
#define HULL_BIT 0x8
#define GEOMETRY_BIT 0x10
#define COMPUTE_BIT 0x20

struct attribute
{
	string name;
	size_t byteSize;
	void* data;
	uint8_t shaders; // 0 0 C G H D P V what Shaders it applies to
	bool externalPointer; // ? do not delete : delete
};

class Material : Resource
{
public:
	static Material* getMaterial(string name);
	~Material();

	void apply();
	void setAttribute(uint8_t shaderMask, string name, void* data, size_t dataSize, bool allocate);

	map<string, Texture*> textures;

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleDomainShader* domainShader;
	SimpleHullShader* hullShader;
	SimpleGeometryShader* geometryShader;
	SimpleComputeShader* computeShader;
private:
	Material(string name);
	map<string, attribute> attributes;
};