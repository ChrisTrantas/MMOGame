#include "Ship.h"

Ship::Ship()
{

}

Ship::~Ship()
{

}

void Ship::awake()
{
	//t = gameObject->getComponent<Transform>();
	//
	//mr->mesh = Mesh::getMesh("Models/geodesicTriSphere20.fbx");
	//mr->material = Material::getMaterial("asteroid");
	//mr->material->textures["diffuse"] = Texture::getTexture("Textures/rock.jpg");
	//mr->material->textures["normalMap"] = Texture::getTexture("Textures/rockNormals.jpg");
	//
	//perlinSeed = RAND_VEC3;
	//float deformity = 3.12f;
	//float magnitude = 0.3f;
	//mr->material->setAttribute(VERTEX_BIT, "deformity", &deformity, sizeof(float), true);
	//mr->material->setAttribute(VERTEX_BIT, "magnitude", &magnitude, sizeof(float), true);

	Input::bindToControl("moveRight", 'L');
	Input::bindToControl("moveUp", 'I');
	Input::bindToControl("moveLeft", 'J');
	Input::bindToControl("moveDown", 'K');
}

void Ship::update(float deltaTime, float totalTime)
{
	if (Input::isControlDown("moveRight"))
	{
		NetworkManager::networkManager->updateData(RIGHT);
	}
	if (Input::isControlDown("moveLeft"))
	{
		NetworkManager::networkManager->updateData(LEFT);
	}
	if (Input::isControlDown("moveUp"))
	{
		NetworkManager::networkManager->updateData(UP);
	}
	if (Input::isControlDown("moveDown"))
	{
		NetworkManager::networkManager->updateData(DOWN);
	}
}

void Ship::draw()
{
	mr->material->setAttribute(VERTEX_BIT, "seed", &perlinSeed, sizeof(vec3), false);
}