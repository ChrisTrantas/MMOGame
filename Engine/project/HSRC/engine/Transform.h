#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
#include "IComponent.h"

using namespace glm;
using namespace std;

class Transform : public IComponent
{
public:
	Transform();
	~Transform();

	__declspec(property(get = getPosition, put = setPosition)) vec3 position;
	__declspec(property(get = getLocalPosition, put = setLocalPosition)) vec3 localPosition;

	__declspec(property(get = getScale, put = setScale)) vec3 scale;

	__declspec(property(get = getRotation, put = setRotation)) quat rotation;
	__declspec(property(get = getLocalRotation, put = setLocalRotation)) quat localRotation;
	
	vec3 getPosition();
	void setPosition(vec3 value);

	vec3 getLocalPosition();
	void setLocalPosition(vec3 value);
	vec3 getScale();
	void setScale(vec3 value);

	quat getRotation();
	void setRotation(quat value);
	quat getLocalRotation();
	void setLocalRotation(quat value);

	void translate(const vec3 &v);
	void translateLocal(const vec3 &v);
	void rotate(const quat &q);
	void rotateLocal(const quat &q);
	void rotateEuler(const vec3 &angles);
	void rotateEulerLocal(const vec3 &angles);
	vec3 getEuler();
	vec3 getEulerLocal();

	vec3 forward();
	vec3 localForward();
	vec3 right();
	vec3 localRight();
	vec3 up();
	vec3 localUp();

	mat4 renderMatrix(bool scaled);
	bool isDirty();
	
	void addChild(Transform* child);
	void removeChild(Transform* child);
	Transform* operator[](int i);
	size_t childCount();
	vector<Transform*> getChildren();
	Transform* getParent();
	bool orphanChildrenOnDestroy;
private:
	vec3 _position, _localPosition;
	vec3 _scale;
	quat _rotation, _localRotation;

	vector<Transform*> children;
	Transform* parent;
	void updateChildren();

	bool dirty, unscaledDirty;
	mat4 matrix, unscaledMatrix;
};

void explodeMat4(mat4 &matrix, float explodedMatrix[16]);
