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
	
}

Material::~Material()
{
	
}