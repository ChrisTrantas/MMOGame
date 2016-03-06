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
	// new shaders per material = bad!
	vertexShader = new SimpleVertexShader(DEVICE, DEVICE_CONTEXT);
	pixelShader = new SimplePixelShader(DEVICE, DEVICE_CONTEXT);
	vertexShader->LoadShaderFile(L"Shaders/VertexShader.cso");
	pixelShader->LoadShaderFile(L"Shaders/PixelShader.cso");
	//diffuse = DEFAULT_TEXTURE;
	//normalMap = DEFAULT_TEXTURE;
	diffuse = Texture::getTexture("Textures/rock.jpg");
	normalMap = Texture::getTexture("Textures/rockNormals.jpg");
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