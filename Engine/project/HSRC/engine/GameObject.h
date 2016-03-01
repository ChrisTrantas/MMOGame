#pragma once

#include <typeindex>
#include "Input.h"
#include "MeshRenderer.h"
#include "IComponent.h"

using namespace std;

class GameObject : Resource
{
public:
	static GameObject* getGameObject();
	static GameObject* getGameObject(string name);
	virtual ~GameObject();

	void earlyUpdate(float deltaTime, float totalTime);
	void update(float deltaTime, float totalTime);
	void draw();

	template<typename T>
	void addComponent(T* component);
	template<typename T>
	T* getComponent();
	template<typename T>
	vector<T*> getComponents();
	
	string name;
	void addTag(string tag);
	void removeTag(string tag);
	void editTag(string tag, string newTag);
	bool hasTag(string tag);
protected:
	GameObject(string name);
private:
	map<type_index, vector<IComponent*>> components;
	vector<string> tags;
protected:
	Transform* transform;
	MeshRenderer* meshRenderer;
};

GameObject* findGameObjectByName(string name);
GameObject* findGameObjectWithTag(string tag);
GameObject* findGameObjectWithTags(size_t numTag, ...);
vector<GameObject*> findGameObjectsWithTag(string tag);
vector<GameObject*> findGameObjectsWithTags(size_t numTag, ...);

void earlyUpdateAllGameObjects(float deltaTime, float totalTime);
void updateAllGameObjects(float deltaTime, float totalTime);
void drawAllGameObjects();
void freeAllGameObjects();

template<typename T>
void GameObject::addComponent(T* component)
{
	if (component->gameObject != NULL)
		return;
	auto typeCheck = components.find(type_index(typeid(T)));
	if (typeCheck == components.end())
		components[type_index(typeid(T))] = vector<IComponent*>();
	
	if (components[type_index(typeid(T))].size() < component->getMaxInstances())
	{
		components[type_index(typeid(T))].push_back(component);
		component->gameObject = this;
		component->awake();
	}
}

template<typename T>
T* GameObject::getComponent()
{
	auto typeCheck = components.find(type_index(typeid(T)));
	if (typeCheck != components.end())
		return (T*)(typeCheck->second[0]);
	return NULL;
}

template<typename T>
vector<T*> GameObject::getComponents()
{
	vector<T*> copiedComponents;
	auto typeCheck = components.find(type_index(typeid(T)));
	if (typeCheck == components.end())
		for (size_t i = 0; i < typeCheck->second.size(); i += 1)
			copiedComponents.push_back((T*)(typeCheck->second[i]));
	return copiedComponents;
}
