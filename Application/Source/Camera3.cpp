#include "Camera3.h"
#include"Application.h"
#define CAMERA_SPEED 10

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{

	this->position = pos;
	this->target = Vector3(target.x , target.y, target.z);
	this->up = up;
	Application::GetCursorPos(&cameraCurrentX, &cameraCurrentY);
	Application::GetCursorPos(&cameraPrevX, &cameraPrevY);
	enableControl = true;
}

void Camera3::Reset()
{
}

void Camera3::Update(double dt)
{
	if (enableControl)
	{
		view = (target - position).Normalized();
		right = view.Cross(up);

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
		if ((cameraCurrentX != cameraPrevX) || (cameraCurrentY != cameraPrevY))
		{
			float xoffset = static_cast<float>(cameraCurrentX - cameraPrevX) * dt;
			float yoffset = static_cast<float>(cameraCurrentY - cameraPrevY) * dt;

			float sensitivity = 2.f;
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

			Application::ResetCursor();
			Application::GetCursorPos(&cameraPrevX, &cameraPrevY);
		}
	}
}

void Camera3::EnableControl()
{
	enableControl = true;
}

void Camera3::DisableControl()
{
	enableControl = false;
}