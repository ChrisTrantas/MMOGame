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
