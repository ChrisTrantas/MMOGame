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
	vertexShader = new SimpleVertexShader(Game::game->getDevice(), Game::game->getDeviceContext());
	pixelShader = new SimplePixelShader(Game::game->getDevice(), Game::game->getDeviceContext());
}

Material::~Material()
{
	delete vertexShader;
	delete pixelShader;
}

SimpleVertexShader* Material::getVertexShader()
{
	return vertexShader;
}

SimplePixelShader* Material::getPixelShader()
{
	return pixelShader;
}