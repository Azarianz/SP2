#pragma once
#include "Mesh.h"
#include <string>
class Entity
{
private:
	Mesh* mesh;
	Vector3 transform;
	Vector3 rotationAxis;
	float rotationAngle;
	Vector3 scale;

public:
	enum class vector3Type
	{
		TRANSFORM,
		SCALE
	};

	Entity();
	~Entity();

	Mesh* getMesh();
	void setMesh(Mesh* mesh);

	Vector3 getScale();
	void setScale(Vector3 scale);

	Vector3 getTransform();
	void setTransform(Vector3 transform);

	Vector3 getRotationAxis();
	void setRotationAxis(Vector3 axis);

	float getRotationAngle();
	void setRotationAngle(float angle);
};

