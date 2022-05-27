#pragma once
#include "CDT.h"

#define COLLISION_OFFSET 0.25f

using namespace glm;

class Transform
{
private:
	mat4		modelMatrix;
public:
	vec3		position;			// usually we will use only x and y
	vec3		scale;				// usually we will use only x and y
	float		orientation;		// 0 radians is 3 o'clock, PI/2 radian is 12 o'clock

	Transform();
	Transform(vec3 pos, vec3 scale,float ori);
	void UpdateModelMat();
	mat4 GetModelMat() { return modelMatrix; }

	bool checkCollision(Transform check);
};

