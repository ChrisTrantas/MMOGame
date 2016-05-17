#include "Light.h"

Light* Light::getLight(string name)
{
	Light* light = (Light*)ResourceManager::manager->getResource("GameObject Light " + name);
	if (!light)
		light = new Light(name);
	return light;
}

Light::Light(string name) : GameObject("Light " + name)
{
	//meshRenderer->visible = false;
	ambientColor = vec4(1, 1, 1, 1);
	diffuseColor = vec4(0.5f, 0.5f, 0.5f, 0.5f);
	fov = 0.75f * (float)M_PI;
	range = 100;
}

Light::~Light()
{

}

DirectionalLight Light::getDirectionalLight()
{
	vec3 f = transform->forward();
	DirectionalLight light =
	{
		DirectX::XMFLOAT4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a),
		DirectX::XMFLOAT4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a),
		DirectX::XMFLOAT3(f.x, f.y, f.z)
	};
	return light;
}

SpotLight Light::getSpotLight()
{
	float viewMat[16];
	explodeMat4(transpose(lookAt(transform->position, transform->position + transform->forward(), transform->up())), viewMat);
	float perspectiveMat[16];
	explodeMat4(transpose(perspective(fov, 1.0f, 0.01f, 1000.0f)), perspectiveMat);
	vec3 f = transform->forward();
	vec3 p = transform->getPosition();

	SpotLight light =
	{
		DirectX::XMFLOAT4X4(viewMat),
		DirectX::XMFLOAT4X4(perspectiveMat),
		DirectX::XMFLOAT3(f.x, f.y, f.z),
		fov,
		DirectX::XMFLOAT3(p.x, p.y, p.z),
		range
	};
	return light;
}
