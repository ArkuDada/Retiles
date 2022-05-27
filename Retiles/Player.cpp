#include "Player.h"

Player::Player(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr):AnimatedObject(active, pos, scale, ori, spr)
{
	isFalling = false;
	isWalking = false;
	superJump = false;
	canDoubleJump = false;
	stickyJump = false;
	canJump = false;
}

void Player::PlayerInput(int input)
{
	
	if ((input & PLAYER_JUMP) && ((!isFalling && canJump)|| canDoubleJump) && !stickyJump)
	{
		transform.position.y += 1.5f;
		body.velocity.y = superJump?SUPER_JUMP_VELOCITY: JUMP_VELOCITY;
		if (!canJump && canDoubleJump)
		{
			canDoubleJump = false;
		}
		isFalling = true;
		canJump = false;
		superJump = false;
		stickyJump = true;
	}
	if (input & PLAYER_LEFT)
	{
		if (transform.scale.x > 0.0f)
		{
			transform.scale.x *= -1.0f;
		}
		body.velocity.x = -(isFalling ? MOVEAIR_VELOCITY_PLAYER : MOVE_VELOCITY_PLAYER);
		isWalking = true;
	}
	else if (input & PLAYER_RIGHT)
	{
		if (transform.scale.x < 0.0f)
		{
			transform.scale.x *= -1.0f;
		}
		body.velocity.x = isFalling? MOVEAIR_VELOCITY_PLAYER:MOVE_VELOCITY_PLAYER;
		isWalking = true;
	}
	else
	{
		body.velocity.x = 0.0f;
		offsetX = 0;
		isWalking = false;
	}

	if (input & PLAYER_STOP_JUMP)
	{
		stickyJump = false;
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
		transform.position.y = (int)transform.position.y - 0.6f;
	}

	//+ Player is on the ground or just landed on the ground
	if (mapCollsionFlag & COLLISION_BOTTOM)
	{
		transform.position.y = (int)transform.position.y + 0.9f;
		body.velocity.y = 0.0f;
		isFalling = false;
		canJump = true;
	}
	//+ Player is jumping/falling
	else
	{
		isFalling = true;
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

	if (canDoubleJump)
	{
		offsetY += 0.25f;
	}
}
