#ifndef CAMERA3_H
#define CAMERA3_H

#include "Camera.h"

class Camera3 : public Camera
{
public:
	float rotateMod;
	bool enableControl;

	double cameraPrevX;
	double cameraPrevY;
	double cameraCurrentX;
	double cameraCurrentY;
	double yaw;
	double pitch;

	Vector3 view;
	Vector3 right;

	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Reset();
	virtual void Update(double dt);
};

#endif