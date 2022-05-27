#pragma once
#include "AnimatedObject.h"
#include "Collision.h"

#define MOVE_VELOCITY_PLAYER 95
#define MOVEAIR_VELOCITY_PLAYER 65
#define JUMP_VELOCITY 100
#define SUPER_JUMP_VELOCITY 200


#define PLAYER_LEFT 1<<0
#define PLAYER_RIGHT 1<<1
#define PLAYER_JUMP 1<<2
#define PLAYER_STOP_JUMP 1<<3

class Player : public AnimatedObject
{
private:
	bool isWalking;
	bool canJump;
	bool stickyJump;
public:
	bool isFalling;
	bool superJump;
	bool canDoubleJump;

	Player(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr);
	void PlayerInput(int input);
	void PlayerMapCollisionBehavior(int collisionFlag);
	void UpdateAnimation() override;
};

