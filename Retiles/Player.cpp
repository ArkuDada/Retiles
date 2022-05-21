#include "Player.h"

Player::Player(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr):AnimatedObject(active, pos, scale, ori, spr)
{
	body = PhysicObject(&transform);
	isJumping = false;
}

void Player::PlayerInput()
{
	if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && (isJumping == false))
	{
		body.velocity.y = JUMP_VELOCITY;
		isJumping = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (transform.scale.x < 0.0f)
		{
			transform.scale.x *= -1.0f;
		}
		body.velocity.x = -MOVE_VELOCITY_PLAYER;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		if (transform.scale.x > 0.0f)
		{
			transform.scale.x *= -1.0f;
		}
		body.velocity.x = MOVE_VELOCITY_PLAYER;
	}
	else
	{
		body.velocity.x = 0.0f;
		offsetX = 0;
	}
	// Play jump animation
	if (isJumping)
	{
		offsetX = 0.75;	// the 4th sprite of the sheet of 4
	}
}
