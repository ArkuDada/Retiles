#include "PhysicObject.h"

PhysicObject::PhysicObject()
{
	tranform = nullptr;
	velocity = vec3(0.0f, 0.0f, 0.0f);
}

PhysicObject::PhysicObject(Transform* t) :tranform(t) {
	velocity = vec3(0.0f, 0.0f, 0.0f);
}
void PhysicObject::UpdatePosByVel(double dt)
{
	tranform->position += velocity * vec3(dt, dt, 0.0f);
}
;