#pragma once

#include "GameObject.h"

#define DEFAULT_LIGHT Light::getLight("DEFAULT")

struct DirectionalLight
{
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 direction;
};

struct SpotLight
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT3 direction;
	float fov;
	float range;
};

class Light : public GameObject
{
public:
	static Light* getLight(string name);
	~Light();
	DirectionalLight getDirectionalLight();
	SpotLight getSpotLight();

	vec4 ambientColor;
	vec4 diffuseColor;
	float fov;
	float range;
private:
	Light(string name);
};
