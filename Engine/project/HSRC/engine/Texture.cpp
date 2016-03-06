#include "Texture.h"
#include "Common.h"
#include "Game.h"

Sampler* Sampler::getSampler(string samplerName)
{
	Sampler* sampler = (Sampler*)ResourceManager::manager->getResource(samplerName);
	if (!sampler)
		sampler = new Sampler(samplerName);
	return sampler;
}

Sampler::Sampler(string samplerName) : Resource(samplerName, MY_TYPE_INDEX)
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	DEVICE->CreateSamplerState(&samplerDesc, &samplerState);
}

Sampler::~Sampler()
{
	ReleaseMacro(samplerState);
}

ID3D11SamplerState* Sampler::getSamplerState()
{
	return samplerState;
}

Texture* Texture::getTexture(string texturePath)
{
	Texture* texture = (Texture*)ResourceManager::manager->getResource(texturePath);
	if (!texture)
		texture = new Texture(texturePath);
	return texture;
}

Texture::Texture(string texturePath) : Resource(texturePath, MY_TYPE_INDEX)
{
	std::wstring wstr = STR_TO_WCHART(texturePath);
	DirectX::CreateWICTextureFromFile(DEVICE, DEVICE_CONTEXT, wstr.c_str(), 0, &srv);
	sampler = DEFAULT_SAMPLER;
}

Texture::~Texture()
{
	ReleaseMacro(srv);
}

ID3D11SamplerState* Texture::getSamplerState()
{
	return sampler->getSamplerState();
}

ID3D11ShaderResourceView* Texture::getSRV()
{
	return srv;
}
