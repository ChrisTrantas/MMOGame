#include "Ship.h"

Ship::Ship()
{

}

Ship::~Ship()
{

}

void Ship::awake()
{
	t = gameObject->getComponent<Transform>();

	Input::bindToControl("moveRight", 'D');
	Input::bindToControl("moveUp", 'W');
	Input::bindToControl("moveLeft", 'A');
	Input::bindToControl("fire", VK_SPACE);
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
	if (Input::isControlDown("fire"))
	{
		NetworkManager::networkManager->updateData(FIRE);
	}

	if (Input::wasControlReleased("moveRight"))
	{
		NetworkManager::networkManager->updateData(INPUT_NONE);
	}
	if (Input::wasControlReleased("moveLeft"))
	{
		NetworkManager::networkManager->updateData(INPUT_NONE);
	}
	if (Input::wasControlReleased("moveUp"))
	{
		NetworkManager::networkManager->updateData(INPUT_NONE);
	}
}

void Ship::move(vec2 moveDir)
{
	t->translate(vec3(moveDir, 0));
}

void Ship::draw()
{
	// apply matierals and textures here
	// note: textures need only be applied once unless they are changing
	// note: attributes need only be applied once if set to "store" or if they are changeing as a "store"
	// note:: if the attribute is always changing, don't "store"

	// "storing" an attribute is like a static variable in the shader
	// not storing is like an instance variable

	//mr->material->setAttribute(VERTEX_BIT, "seed", &perlinSeed, sizeof(vec3), false);
	//printf("working\n");
}