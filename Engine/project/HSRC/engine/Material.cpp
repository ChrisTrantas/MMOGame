#include "Material.h"
#include "Game.h"

Material* Material::getMaterial(string name)
{
	Material* material = (Material*)ResourceManager::manager->getResource("Material " + name);
	if (!material)
		material = new Material(name);
	return material;
}

Material::Material(string name) : Resource("Material " + name, MY_TYPE_INDEX)
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	domainShader = nullptr;
	hullShader = nullptr;
	geometryShader = nullptr;
	computeShader = nullptr;
}

Material::~Material()
{
	auto attribute = attributes.begin();
	while (attribute != attributes.end())
	{
		if (!attribute->second.externalPointer)
			delete attribute->second.data;
		++attribute;
	}
}

void Material::apply()
{
	auto texture = textures.begin();

	// all textures use first texture's sampler for now
	if (texture != textures.end())
		pixelShader->SetSamplerState("trilinear", textures[texture->first]->getSamplerState());

	while (texture != textures.end())
	{
		pixelShader->SetShaderResourceView(texture->first.c_str(), texture->second->getSRV());
		++texture;
	}

	auto attribute = attributes.begin();
	while (attribute != attributes.end())
	{
		if ((attribute->second.shaders & VERTEX_BIT) && vertexShader)
			vertexShader->SetData(attribute->second.name, attribute->second.data, attribute->second.byteSize);
		if ((attribute->second.shaders & PIXEL_BIT) && pixelShader)
			pixelShader->SetData(attribute->second.name, attribute->second.data, attribute->second.byteSize);
		++attribute;
	}

	if (vertexShader)
		vertexShader->SetShader(true);
	if (pixelShader)
		pixelShader->SetShader(true);
}

void Material::setAttribute(uint8_t shaderMask, string name, void* data, size_t dataSize, bool allocate)
{
	if (allocate)
	{
		auto attribute = attributes.find(name);
		if (attribute != attributes.end() && !attribute->second.externalPointer)
			delete attribute->second.data;

		attributes[name].name = name;
		attributes[name].byteSize = dataSize;
		attributes[name].data = new uint8_t[dataSize];
		memcpy_s(attributes[name].data, dataSize, data, dataSize);
		attributes[name].shaders = shaderMask;
		attributes[name].externalPointer = false;
	}
	else
		attributes[name] = {name, dataSize, data, shaderMask, true};
}
