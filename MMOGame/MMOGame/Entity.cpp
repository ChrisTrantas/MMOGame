#include "Entity.h"



Entity::Entity(Mesh* m)
{
	XMMATRIX world = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(world));

	XMVECTOR pos = XMVECTOR();
	XMStoreFloat3(&position, pos);

	XMVECTOR rot = XMVECTOR();
	XMStoreFloat3(&rotation, rot);

	XMVECTOR size = XMVECTOR();
	XMStoreFloat3(&scale, size);

	mesh = m;
}


Entity::~Entity()
{
}

XMFLOAT3 Entity::getPosition()
{
	return position;
}

void Entity::setPosition(XMFLOAT3 pos)
{
	position = pos;
}

XMFLOAT3 Entity::getRotation()
{
	return rotation;
}

void Entity::setRotation(XMFLOAT3 rot)
{
	rotation = rot;
}

XMFLOAT3 Entity::getScale()
{
	return scale;
}

void Entity::setScale(XMFLOAT3 scl)
{
	scale = scl;
}
