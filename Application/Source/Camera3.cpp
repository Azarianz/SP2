#include "Camera3.h"
#include"Application.h"
#define CAMERA_SPEED 10

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{

	this->position = pos;
	this->target = Vector3(position.x, position.y, position.z + 0.5);
	this->up = up;
	enableControl = true;

	rotateMod = 0;
}

void Camera3::Reset()
{
}

void Camera3::Update(double dt)
{
	view = (target - position).Normalized();
	right = view.Cross(up);

	if (enableControl)
	{
		rotateMod = 50 * dt;
		//camera move controls
		if (Application::IsKeyPressed('A'))
		{
			position -= right * CAMERA_SPEED * dt;
			float previousY = target.y;
			target = position + view;
			target.y = previousY;
		}
		if (Application::IsKeyPressed('D'))
		{
			position += right * CAMERA_SPEED * dt;
			float previousY = target.y;
			target = position + view;
			target.y = previousY;
		}
		if (Application::IsKeyPressed('S'))
		{
			float temp = view.y;
			view.y = 0;
			position -= view * CAMERA_SPEED * dt;
			view.y = temp;
			float previousY = target.y;
			target = position + view;
			target.y = previousY;
		}
		if (Application::IsKeyPressed('W'))
		{
			float temp = view.y;
			view.y = 0;
			position += view * CAMERA_SPEED * dt;
			view.y = temp;
			float previousY = target.y;
			target = position + view;
			target.y = previousY;

		}

		//camera look controls
		if ((Application::IsKeyPressed(VK_UP)) && (view.y < 0.5))
		{
			Mtx44 rotation;
			rotation.SetToRotation(rotateMod, right.x, right.y, right.z);
			view = rotation * view;
			target = position + view;
		}
		if ((Application::IsKeyPressed(VK_DOWN)) && (view.y > -0.5))
		{
			Mtx44 rotation;
			rotation.SetToRotation(-rotateMod, right.x, right.y, right.z);
			view = rotation * view;
			target = position + view;
		}
		if (Application::IsKeyPressed(VK_LEFT))
		{
			Mtx44 rotation;
			rotation.SetToRotation(rotateMod, 0, 1, 0);
			view = rotation * view;
			target = position + view;
		}
		if (Application::IsKeyPressed(VK_RIGHT))
		{
			Mtx44 rotation;
			rotation.SetToRotation(-rotateMod, 0, 1, 0);
			view = rotation * view;
			target = position + view;
		}
	}
}