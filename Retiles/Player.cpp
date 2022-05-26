#include "Player.h"

Player::Player(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr):AnimatedObject(active, pos, scale, ori, spr)
{
	body = PhysicObject(&transform);
	isJumping = false;
	isWalking = false;
}

void Player::PlayerInput(int input)
{
	if ((input & PLAYER_JUMP) && (!isJumping))
	{
		body.velocity.y = JUMP_VELOCITY;
		isJumping = true;
	}
	if (input & PLAYER_LEFT)
	{
		if (transform.scale.x > 0.0f)
		{
			transform.scale.x *= -1.0f;
		}
		body.velocity.x = -(isJumping ? MOVEAIR_VELOCITY_PLAYER : MOVE_VELOCITY_PLAYER);
		isWalking = true;
	}
	else if (input & PLAYER_RIGHT)
	{
		if (transform.scale.x < 0.0f)
		{
			transform.scale.x *= -1.0f;
		}
		body.velocity.x = isJumping? MOVEAIR_VELOCITY_PLAYER:MOVE_VELOCITY_PLAYER;
		isWalking = true;
	}
	else
	{
		body.velocity.x = 0.0f;
		offsetX = 0;
		isWalking = false;
	}
	// Play jump animation
	if (isJumping)
	{
		//offsetX = 0.75;	// the 4th sprite of the sheet of 4
	}
}

void Player::PlayerMapCollisionBehavior(int mapCollsionFlag)
{
	// Collide Left
	if (mapCollsionFlag & COLLISION_LEFT)
	{
		transform.position.x = (int)transform.position.x + 1.0f;
	}

	//+ Collide Right
	if (mapCollsionFlag & COLLISION_RIGHT)
	{
		transform.position.x = (int)transform.position.x -1.0f;
	}

	//+ Collide Top
	if (mapCollsionFlag & COLLISION_TOP)
	{
		transform.position.y = (int)transform.position.y + 0.5f;
		body.velocity.y = 0.0f;
	}

	//+ Player is on the ground or just landed on the ground
	if (mapCollsionFlag & COLLISION_BOTTOM)
	{
		transform.position.y = (int)transform.position.y + 0.75f;
		body.velocity.y = 0.0f;
		isJumping = false;
	}
	//+ Player is jumping/falling
	
	else
	{
		//isJumping = true;
	}
}

void Player::UpdateAnimation()
{
	frame++;
	if (frame % ANIM_SPEED != 0)
	{
		return;
	}

	if (isWalking)
	{
		offsetY = 0.5f;
		numFrame = 4;
		curFrame = (curFrame++) % numFrame;
		offsetX = (float)curFrame / numFrame;
	}
	else
	{
		offsetX = 0.0f;
		offsetY = 0.0f;
	}
}
