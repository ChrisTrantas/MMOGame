#include "TestComponent.h"

TestComponent::TestComponent()
{
	
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
	t->rotate(angleAxis(angle, t->up()));
}
