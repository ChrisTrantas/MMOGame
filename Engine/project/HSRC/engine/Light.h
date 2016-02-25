#pragma once

#include "GameObject.h"

#define DEFAULT_LIGHT Light::getLight("DEFAULT")

struct DirectionalLight
{
	DirectX::XMFLOAT4 AmbientColor;
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 Direction;
};

class Light : public GameObject
{
public:
	static Light* getLight(string name);
	~Light();
	DirectionalLight getDirectionalLight();

	vec4 ambientColor;
	vec4 diffuseColor;
private:
	Light(string name);
};
