#include <stdarg.h>
#include "GameObject.h"
#include "Camera.h"

static vector<GameObject*> objects;
static unsigned int anonID = 0;

GameObject* GameObject::getGameObject()
{
	string name = string("Anonymous") + to_string(anonID);
	GameObject* gameObject = (GameObject*)ResourceManager::manager->getResource("GameObject " + name);
	if (!gameObject)
	{
		gameObject = new GameObject(name);
		++anonID;
	}
	return gameObject;
}

GameObject* GameObject::getGameObject(string name)
{
	GameObject* gameObject = (GameObject*)ResourceManager::manager->getResource("GameObject " + name);
	if (!gameObject)
		gameObject = new GameObject(name);
	return gameObject;
}

GameObject::GameObject(string name) : Resource("GameObject " + name, MY_TYPE_INDEX)
{
	components = map<type_index, vector<IComponent*>>();
	transform = new Transform();
	addComponent<Transform>(transform);
	meshRenderer = new MeshRenderer();
	addComponent<MeshRenderer>(meshRenderer);
	this->name = name;
	tags = vector<string>();
	objects.push_back(this);
}

GameObject::~GameObject()
{
	auto componentBucket = components.begin();
	while (componentBucket != components.end())
	{
		while (componentBucket->second.size() > 0)
		{
			IComponent* ic = componentBucket->second.back();
			componentBucket->second.pop_back();
			delete ic;
		}
		componentBucket++;
	}

	auto index = find(objects.begin(), objects.end(), this);
	objects.erase(objects.begin() + (index - objects.begin()));
}

void GameObject::addTag(string tag)
{
	if (!hasTag(tag))
		tags.push_back(tag);
}

void GameObject::removeTag(string tag)
{
	auto index = find(tags.begin(), tags.end(), tag);
	if (index != tags.end())
		tags.erase(tags.begin() + (index - tags.begin()));
}

void GameObject::editTag(string tag, string newTag)
{
	auto index = find(tags.begin(), tags.end(), tag);
	if (index != tags.end())
		tags[index - tags.begin()] = newTag;
}

bool GameObject::hasTag(string tag)
{
	auto index = find(tags.begin(), tags.end(), tag);
	if (index != tags.end())
		return true;
	return false;
}

void GameObject::earlyUpdate()
{
	auto componentBucket = components.begin();
	while (componentBucket != components.end())
	{
		for (size_t i = 0; i < componentBucket->second.size(); i += 1)
		{
			if (componentBucket->second[i]->enabled)
				componentBucket->second[i]->earlyUpdate();
		}
		componentBucket++;
	}
}

void GameObject::update()
{
	auto componentBucket = components.begin();
	while (componentBucket != components.end())
	{
		for (size_t i = 0; i < componentBucket->second.size(); i += 1)
		{
			if (componentBucket->second[i]->enabled)
				componentBucket->second[i]->update();
		}
		componentBucket++;
	}
}

void GameObject::draw()
{
	auto componentBucket = components.begin();
	while (componentBucket != components.end())
	{
		for (size_t i = 0; i < componentBucket->second.size(); i += 1)
		{
			if (componentBucket->second[i]->enabled)
				componentBucket->second[i]->draw();
		}
		componentBucket++;
	}
	meshRenderer->draw(DEFAULT_CAMERA, transform);
}

GameObject* findGameObjectByName(string name)
{
	for (size_t i = 0; i < objects.size(); ++i)
		if (objects[i]->name == name)
			return objects[i];

	return NULL;
}

GameObject* findGameObjectWithTag(string tag)
{
	for (size_t i = 0; i < objects.size(); ++i)
		if (objects[i]->hasTag(tag))
			return objects[i];
	
	return NULL;
}

GameObject* findGameObjectWithTags(size_t numTag, ...)
{
	vector<string> filter = vector<string>(numTag);
	va_list args;
	va_start(args, numTag);

	for (size_t i = 0; i < numTag; ++i)
		filter[i] = va_arg(args, char*);

	for (size_t i = 0; i < objects.size(); ++i)
	{
		for (size_t j = 0; j < numTag; ++j)
		{
			if (!objects[i]->hasTag(filter[j]))
				break;
			if (j == numTag - 1)
			{
				va_end(args);
				return objects[i];
			}
		}
	}

	va_end(args);
	return NULL;
}

vector<GameObject*> findGameObjectsWithTag(string tag)
{
	vector<GameObject*> taggedObjects;

	for (size_t i = 0; i < objects.size(); ++i)
		if (objects[i]->hasTag(tag))
			taggedObjects.push_back(objects[i]);

	return taggedObjects;
}

vector<GameObject*> findGameObjectsWithTags(size_t numTag, ...)
{
	vector<GameObject*> taggedObjects;
	vector<string> filter = vector<string>(numTag);
	va_list args;
	va_start(args, numTag);

	for (size_t i = 0; i < numTag; ++i)
		filter[i] = va_arg(args, char*);

	for (size_t i = 0; i < objects.size(); ++i)
	{
		for (size_t j = 0; j < numTag; ++j)
		{
			if (!objects[i]->hasTag(filter[j]))
				break;
			if (j == numTag - 1)
				taggedObjects.push_back(objects[i]);
		}
	}

	va_end(args);
	return taggedObjects;
}

void earlyUpdateAllGameObjects()
{
	for (size_t i = 0; i < objects.size(); i += 1)
		objects[i]->earlyUpdate();
}

void updateAllGameObjects()
{
	for (size_t i = 0; i < objects.size(); i += 1)
		objects[i]->update();
}

void drawAllGameObjects()
{
	for (size_t i = 0; i < objects.size(); i += 1)
			objects[i]->draw();
}

void freeAllGameObjects()
{
	while (objects.size() > 0)
		delete objects[0];
}
