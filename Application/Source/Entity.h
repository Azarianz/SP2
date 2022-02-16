#pragma once
#include "Mesh.h"
#include <string>
class Entity
{
private:
	Mesh* mesh;
	Vector3 transform;
	Vector3 scale;

public:
	enum class vector3Type
	{
		TRANSFORM,
		SCALE
	};

	Entity();
	Entity(Mesh* mesh);
	Entity(Mesh* mesh, Vector3 vector, vector3Type type);
	Entity(Mesh* mesh, Vector3 transform, Vector3 scale);
	~Entity();

	Mesh* getMesh();
	void setMesh(Mesh* mesh);

	Vector3 getScale();
	void setScale(Vector3 scale);

	Vector3 getTransform();
	void setTransform(Vector3 transform);
};

