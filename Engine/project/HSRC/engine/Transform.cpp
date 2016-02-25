#include <algorithm>
#include "Transform.h"

Transform::Transform()
{
	_position = vec3(0, 0, 0);
	_localPosition = _position;
	_scale = vec3(1, 1, 1);
	_rotation = quat();
	_localRotation = _rotation;
	parent = NULL;
	orphanChildrenOnDestroy = false;
	dirty = true;
}

Transform::~Transform()
{
	while (children.size() > 0)
	{
		if (!orphanChildrenOnDestroy)
		{
			if (parent != NULL)
				parent->addChild(children[0]);
			else
				removeChild(children[0]);
		}
		else
			removeChild(children[0]);
	}
}

void Transform::translate(const vec3 &v) { position = _position + v; }
void Transform::translateLocal(const vec3 &v) { localPosition = _localPosition + v; }

void Transform::rotate(const quat &q) { rotation = normalize(q * _rotation); }
void Transform::rotateLocal(const quat &q) { localRotation = normalize(_localRotation * q); }

void Transform::rotateEuler(const vec3 &angles) { rotation = normalize(quat(angles) * _rotation); }
void Transform::rotateEulerLocal(const vec3 &angles) { localRotation = normalize(_localRotation * quat(angles)); }

vec3 Transform::getEuler() { return eulerAngles(_rotation); }
vec3 Transform::getEulerLocal() { return eulerAngles(_localRotation); }

vec3 Transform::forward() { return _rotation * vec3(0, 0, -1); }
vec3 Transform::localForward() { return _localRotation * vec3(0, 0, -1); }
vec3 Transform::right() { return _rotation * vec3(1, 0, 0); }
vec3 Transform::localRight() { return _localRotation * vec3(1, 0, 0); }
vec3 Transform::up() { return _rotation * vec3(0, 1, 0); }
vec3 Transform::localUp() { return _localRotation * vec3(0, 1, 0); }

mat4 Transform::renderMatrix(bool scaled)
{
	if (dirty && scaled)
	{
		matrix = glm::translate(_position) * mat4_cast(_rotation) * glm::scale(_scale);
		dirty = false;
	}

	if (unscaledDirty && !scaled)
	{
		unscaledMatrix = glm::translate(_position) * mat4_cast(_rotation);
		unscaledDirty = false;
	}

	if (scaled)
		return matrix;
	return unscaledMatrix;
}

bool Transform::isDirty()
{
	return dirty || unscaledDirty;
}

void Transform::addChild(Transform* child)
{
	if (find(children.begin(), children.end(), child) == children.end() && child != this)
	{
		if (child->parent != 0)
			child->parent->removeChild(child);
		child->parent = this;
		children.push_back(child);
		child->localPosition = (child->position - _position) * _rotation;
		child->localRotation = inverse(_rotation) * child->rotation;
	}
}

void Transform::removeChild(Transform* child)
{
	auto childPos = find(children.begin(), children.end(), child);
	if (childPos != children.end())
	{
		child->parent = 0;
		children.erase(children.begin() + (childPos - children.begin()));
		child->localPosition = child->position;
		child->localRotation = child->rotation;
	}
}

Transform* Transform::operator[](int i)
{
	if (i >= 0 && (size_t)i < children.size())
		return children[i];
	return NULL;
}

size_t Transform::childCount()
{
	return children.size();
}

vector<Transform*> Transform::getChildren()
{
	vector<Transform*> childrenCopy = children;
	return childrenCopy;
}

Transform* Transform::getParent()
{
	return parent;
}

vec3 Transform::getPosition() { return _position; }
void Transform::setPosition(vec3 value)
{
	_position = value;

	if (parent != NULL)
		_localPosition = _position * parent->rotation - parent->position;
	else
		_localPosition = _position;

	updateChildren();

	dirty = true;
	unscaledDirty = true;
}

vec3 Transform::getLocalPosition() { return _localPosition; }
void Transform::setLocalPosition(vec3 value)
{
	_localPosition = value;

	if (parent != NULL)
		_position = parent->position + parent->rotation * _localPosition;
	else
		_position = _localPosition;

	updateChildren();

	dirty = true;
	unscaledDirty = true;
}

vec3 Transform::getScale() { return _scale; }
void Transform::setScale(vec3 value)
{
	_scale = value;

	dirty = true;
}

quat Transform::getRotation() { return _rotation; }
void Transform::setRotation(quat value)
{
	_rotation = value;

	if (parent != NULL)
		_localRotation = parent->rotation * _rotation;
	else
		_localRotation = _rotation;

	updateChildren();

	dirty = true;
	unscaledDirty = true;
}

quat Transform::getLocalRotation() { return _localRotation; }
void Transform::setLocalRotation(quat value)
{
	_localRotation = value;

	if (parent != NULL)
		_rotation = parent->rotation * _localRotation;
	else
		_rotation = _localRotation;

	updateChildren();

	dirty = true;
	unscaledDirty = true;
}

void Transform::updateChildren()
{
	for (size_t i = 0; i < children.size(); i += 1)
	{
		children[i]->localRotation = children[i]->localRotation;
		children[i]->localPosition = children[i]->localPosition;
	}
}

void explodeMat4(mat4 &matrix, float explodedMatrix[16])
{
	for (size_t i = 0; i < 16; ++i)
		explodedMatrix[i] = matrix[i/4][i%4];
}
