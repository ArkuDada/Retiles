#pragma once
#include "CDT.h"

class GameObject
{
public:
	CDTMesh* mesh;
	CDTTex* tex;
	int				type;				// enum GAMEOBJ_TYPE
	int				flag;				// 0 - inactive, 1 - active
	glm::vec3		position;			// usually we will use only x and y
	glm::vec3		velocity;			// usually we will use only x and y
	glm::vec3		scale;				// usually we will use only x and y
	float			orientation;		// 0 radians is 3 o'clock, PI/2 radian is 12 o'clock
	glm::mat4		modelMatrix;
	GameObject*		target;

	int value;
	int health;
	float damage;

	GameObject(CDTMesh* , CDTTex* ,int ,int , glm::vec3 , glm::vec3 , glm::vec3 , float );
};

