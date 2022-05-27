#include "Camera.h"
#include <iostream>
Camera::Camera(Transform* _track)
{
	track = _track;

	 minX = CELL_SIZE * 5;
	 minY = CELL_SIZE * 5;

	 maxX = 0.0f;
	 maxY = 0.0f;

	position = glm::vec3(minX, minY, 0);
}

void Camera::SetCameraLimit(Cordinate mapSize)
{
	int countX = (int)mapSize.x / 9; //1,2,3
	int countY = (int)mapSize.x / 9;

	maxX = CELL_SIZE * (5 + 10 * (countX - 1));
	maxY = CELL_SIZE * (5 + 10 * (countY - 1));
}

void Camera::UpdateCamera()
{
	position = track->position;
	if (position.x < minX)
	{
		position.x = minX;
	}
	if (position.x > maxX)
	{
		position.x = maxX;
	}

	if (position.y < minY)
	{
		position.y = minY;
	}
	if (position.y > maxY)
	{
		position.y = maxY;
	}

	SetCamPosition(position.x, position.y);
}
