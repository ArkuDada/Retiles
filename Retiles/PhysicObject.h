#pragma once
#include "CDT.h"
#include "Transform.h"
using namespace glm;

class PhysicObject
{
private:
	Transform* tranform;

public:	
	vec3 velocity;

	PhysicObject();
	PhysicObject(Transform* tranform);
	void UpdatePosByVel(double dt);
};

