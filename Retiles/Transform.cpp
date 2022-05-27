#include "Transform.h"

Transform::Transform()
{
	position = vec3(0.0f,0.0f, 0.0f);
	scale = vec3(1.0f, 1.0f, 0.0f);
	orientation = 0.0f;
	modelMatrix = mat4(1.0f);
}

Transform::Transform(vec3 p, vec3 s, float o):position(p),scale(s),orientation(o)
{
	modelMatrix = mat4(1.0f);
}

void Transform::UpdateModelMat()
{
		mat4 rMat = mat4(1.0f);
		mat4 sMat = mat4(1.0f);
		mat4 tMat = mat4(1.0f);

		// Compute the scaling matrix
		sMat = glm::scale(mat4(1.0f), scale);

		//+ Compute the rotation matrix, we should rotate around z axis 
		rMat = rotate(mat4(1.0f), radians(orientation), vec3(0.0f, 0.0f, 1.0f));

		//+ Compute the translation matrix
		tMat = translate(mat4(1.0f), position);

		// Concatenate the 3 matrix to from Model Matrix
		modelMatrix = tMat * sMat * rMat;
}

bool Transform::checkCollision(Transform check)
{
	float rightA = position.x + (fabs(scale.x) / 2.0f) - COLLISION_OFFSET;
	float leftA = position.x - (fabs(scale.x) / 2.0f) + COLLISION_OFFSET;
	float topA = position.y + (fabs(scale.y) / 2.0f) - COLLISION_OFFSET;
	float bottomA = position.y - (fabs(scale.y) / 2.0f) + COLLISION_OFFSET;

	float rightB = check.position.x + (fabs(check.scale.x) / 2.0f) - COLLISION_OFFSET;
	float leftB = check.position.x - (fabs(check.scale.x) / 2.0f) + COLLISION_OFFSET;
	float topB = check.position.y + (fabs(check.scale.y) / 2.0f) - COLLISION_OFFSET;
	float bottomB = check.position.y - (fabs(check.scale.y) / 2.0f) + COLLISION_OFFSET;

	return !(leftA > rightB || leftB > rightA || bottomA > topB || bottomB > topA);
}
