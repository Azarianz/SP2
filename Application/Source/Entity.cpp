#include "Entity.h"

Entity::Entity()
{
	mesh = nullptr;
	transform = (0.f, 0.f, 0.f);
	rotationAxis = Vector3(0, 1, 0);
	rotationAngle = 0;
	scale = (1.f, 1.f, 1.f);
}

Entity::~Entity()
{
}

Mesh* Entity::getMesh()
{
	return mesh;
}

void Entity::setMesh(Mesh* mesh)
{
	this->mesh = mesh;
}

Vector3 Entity::getScale()
{
	return scale;
}

void Entity::setScale(Vector3 scale)
{
	this->scale = scale;
}

Vector3 Entity::getTransform()
{
	return transform;
}

void Entity::setTransform(Vector3 transform)
{
	this->transform = transform;
}

Vector3 Entity::getRotationAxis()
{
	return rotationAxis;
}

void Entity::setRotationAxis(Vector3 axis)
{
	rotationAxis = axis;
}

float Entity::getRotationAngle()
{
	return rotationAngle;
}

void Entity::setRotationAngle(float angle)
{
	rotationAngle = angle;
}
