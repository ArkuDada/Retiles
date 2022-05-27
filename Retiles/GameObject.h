#pragma once
#include "CDT.h"
#include "Transform.h"
#include "Sprite.h"
#include "PhysicObject.h"

class GameObject
{
protected:
				
	Sprite*		sprite;

public:
	bool		isActive;
	int			tag;
	Transform	transform;
	PhysicObject body;

	GameObject(bool active,glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr);
	virtual void UpdateBehavior() { return; };
	virtual void Draw();
};

