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

		Application::GetCursorPos(&cameraCurrentX, &cameraCurrentY);
		//camera look controls
		if ((cameraCurrentX != cameraPrevX) && (cameraCurrentY != cameraPrevY))
		{
			float xoffset = cameraCurrentX - cameraPrevX;
			float yoffset = cameraCurrentY - cameraPrevY;

			float sensitivity = 0.1f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 45.0f)
			{
				pitch = 45.0f;
			}
			if (pitch < -45.0f)
			{
				pitch = -45.0f;
			}

			view.x = cos(Math::DegreeToRadian(yaw)) * cos(Math::DegreeToRadian(pitch));
			view.y = -(sin(Math::DegreeToRadian(pitch)));
			view.z = sin(Math::DegreeToRadian(yaw)) * cos(Math::DegreeToRadian(pitch));
			view = view.Normalized();
			target = position + view;

			Application::GetCursorPos(&cameraPrevX, &cameraPrevY);
		}


		/*if ((Application::IsKeyPressed(VK_UP)) && (view.y < 0.5))
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
		}*/
	}
}