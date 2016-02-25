#include "ResourceManager.h"

ResourceManager* ResourceManager::manager = nullptr;

void ResourceManager::init()
{
	if (manager == nullptr)
		manager = new ResourceManager();
}

ResourceManager::ResourceManager()
{
	resources = map<string, Resource*>();
}

ResourceManager::~ResourceManager()
{
	auto resourceSweeper = resources.begin();
	while (resourceSweeper != resources.end())
	{
		delete resourceSweeper->second;
		resourceSweeper++;
	}
}

void ResourceManager::shutdown()
{
	delete manager;
}

void ResourceManager::bindResource(string tag, Resource* resource)
{
	auto tagCheck = resources.find(tag);
	if (tagCheck == resources.end())
		resources[tag] = resource;
}

Resource* ResourceManager::getResource(string tag)
{
	auto tagCheck = resources.find(tag);
	if (tagCheck != resources.end())
		return tagCheck->second;
	return nullptr;
}
