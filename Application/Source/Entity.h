#pragma once
#include "Mesh.h"
class Entity
{
private:
	Mesh* mesh;
	Vector3 transform;
	Vector3 scale;

public:
	Entity(Mesh mesh);
	Entity(Mesh mesh, Vector3 transform);
	Entity(Mesh mesh, Vector3 scale);
	Entity(Mesh mesh, Vector3 transform, Vector3 scale);
	~Entity();

	Mesh* getMesh();
	void setMesh(Mesh mesh);

	Vector3 getScale();
	void setScale(Vector3 scale);

	Vector3 getTransform();
	void setTransform(Vector3 transform);
};

