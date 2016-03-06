#include "TestComponent.h"

TestComponent::TestComponent(float speed)
{
	this->speed = speed;
}

TestComponent::~TestComponent()
{

}

void TestComponent::awake()
{
	angle = 0.0001f;
	t = gameObject->getComponent<Transform>();
}

void TestComponent::update(float deltaTime, float totalTime)
{
	t->rotate(angleAxis(angle * speed, t->up()));
}
