#pragma once
#include <typeindex>
#include <string>

using namespace std;

#define MY_TYPE_INDEX &type_index(typeid(this))

class Resource
{
public:
	virtual ~Resource() {}
protected:
	Resource(string tag, type_index* type);
private:
	string tag;
	type_index* type;
};