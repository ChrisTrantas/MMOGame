#pragma once

#include "ResourceManager.h"
#include "WICTextureLoader.h"

#define DEFAULT_TEXTURE Texture::getTexture("Textures/default.png")
#define DEFAULT_SAMPLER Sampler::getSampler("wrap")

class Sampler : Resource
{
public:
	static Sampler* getSampler(string samplerName);
	~Sampler();
	ID3D11SamplerState* getSamplerState();
private:
	Sampler(string samplerName);
	ID3D11SamplerState* samplerState;
};

class Texture : Resource
{
public:
	static Texture* getTexture(string texturePath);
	~Texture();
	ID3D11SamplerState* getSamplerState();
	ID3D11ShaderResourceView* getSRV();
	Sampler* sampler;
private:
	Texture(string texturePath);
	ID3D11ShaderResourceView* srv;
};
