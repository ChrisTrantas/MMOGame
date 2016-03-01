#pragma once

class GameObject;

class IComponent
{
public:
	virtual ~IComponent() {};
	virtual void awake() {};
	virtual void earlyUpdate(float deltaTime, float totalTime) {};
	virtual void update(float deltaTime, float totalTime) {};
	virtual void draw() {};
	bool enabled = true;
	GameObject* gameObject = (GameObject*)0;
	size_t getMaxInstances() { return maxInstances; }
protected:
	size_t maxInstances = 1;
};
