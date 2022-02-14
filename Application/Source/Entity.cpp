#include "Entity.h"

Entity::Entity(Mesh mesh)
{
	this->mesh = &mesh;
	transform = (0.f, 0.f, 0.f);
	scale = (1.f, 1.f, 1.f);
}

Entity::Entity(Mesh mesh, Vector3 transform)
{
	this->mesh = &mesh;
	this->transform = transform;
	scale = (1.f, 1.f, 1.f);
}

Entity::Entity(Mesh mesh, Vector3 scale)
{
	this->mesh = &mesh;
	transform = (0.f, 0.f, 0.f);
	this->scale = scale;
}

Entity::Entity(Mesh mesh, Vector3 transform, Vector3 scale)
{
	this->mesh = &mesh;
	this->transform = transform;
	this->scale = scale;
}

Entity::~Entity()
{
}

Mesh* Entity::getMesh()
{
	return mesh;
}

void Entity::setMesh(Mesh mesh)
{
	this->mesh = &mesh;
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
