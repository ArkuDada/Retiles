#pragma once
#include "AnimatedObject.h"
#include "PhysicObject.h"

#define MOVE_VELOCITY_PLAYER 50
#define JUMP_VELOCITY 50

class Player : public AnimatedObject
{
private:
	bool isJumping;
	
public:
	PhysicObject body;

	Player(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr);
	void PlayerInput();
};

