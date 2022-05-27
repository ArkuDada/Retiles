#pragma once
#include "Transform.h"
#include "Level.h"
class Camera
{
private:
	glm::vec3 position;

	int minX;
	int maxX;

	int minY;
	int maxY;
		
	Transform* track;
public:
	Camera(Transform* _track);
	void SetCameraLimit(Cordinate mapSize);
	void UpdateCamera();
};

