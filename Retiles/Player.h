#pragma once
#include "AnimatedObject.h"
#include "PhysicObject.h"
#include "Collision.h"

#define MOVE_VELOCITY_PLAYER 75
#define MOVEAIR_VELOCITY_PLAYER 45
#define JUMP_VELOCITY 100

#define PLAYER_LEFT 1<<0
#define PLAYER_RIGHT 1<<1
#define PLAYER_JUMP 1<<2

class Player : public AnimatedObject
{
private:
	bool isWalking;
	
public:
	bool isJumping;
	PhysicObject body;

	Player(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr);
	void PlayerInput(int input);
	void PlayerMapCollisionBehavior(int collisionFlag);
	void UpdateAnimation() override;
};

