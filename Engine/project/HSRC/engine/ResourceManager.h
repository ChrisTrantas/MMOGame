#pragma once
#include <map>
#include "Resource.h"

using namespace std;

class ResourceManager
{
public:
	static void init();
	static ResourceManager* manager;
	void shutdown();

	Resource* getResource(string tag);
	void bindResource(string tag, Resource* resource);
private:
	ResourceManager();
	~ResourceManager();
	ResourceManager(ResourceManager const&) = delete;
	void operator=(ResourceManager const&) = delete;

	map<string, Resource*> resources;
};
