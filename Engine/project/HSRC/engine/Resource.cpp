#include "Resource.h"
#include "ResourceManager.h"

Resource::Resource(string tag, type_index* type)
{
	this->tag = tag;
	this->type = type;
	ResourceManager::manager->bindResource(tag, this);
}
