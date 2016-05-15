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

	Input::bindToControl("moveRight", 'L');
	Input::bindToControl("moveUp", 'I');
	Input::bindToControl("moveLeft", 'J');
	Input::bindToControl("moveDown", 'K');
	Input::bindToControl("fire", 'E');
}

void Ship::update(float deltaTime, float totalTime)
{
	if (Input::isControlDown("moveRight"))
	{
		NetworkManager::networkManager->updateData(RIGHT);
		//move(vec2(1, 0));
	}
	if (Input::isControlDown("moveLeft"))
	{
		NetworkManager::networkManager->updateData(LEFT);
		//move(vec2(-1, 0));
	}
	if (Input::isControlDown("moveUp"))
	{
		NetworkManager::networkManager->updateData(UP);
		//move(vec2(0, 1));
	}
	if (Input::isControlDown("fire"))
	{
		NetworkManager::networkManager->updateData(FIRE);
		//move(vec2(0, 1));
	}

	if (Input::wasControlReleased("moveRight"))
	{
		NetworkManager::networkManager->updateData(RIGHT);
		//move(vec2(1, 0));
	}
	if (Input::wasControlReleased("moveLeft"))
	{
		NetworkManager::networkManager->updateData(LEFT);
		//move(vec2(-1, 0));
	}
	if (Input::wasControlReleased("moveUp"))
	{
		NetworkManager::networkManager->updateData(UP);
		//move(vec2(0, 1));
	}

	//You cant move down in asteroids
	/*if (Input::isControlDown("moveDown"))
	{
		NetworkManager::networkManager->updateData(DOWN);
		move(vec2(0, -1));
	}*/
}

void Ship::move(vec2 moveDir)
{
	//t->position = t->position + vec3(moveDir,0);
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