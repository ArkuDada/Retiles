#include "GameObject.h"

GameObject::GameObject(bool active, glm::vec3 pos, glm::vec3 _scale, float ori, Sprite* spr)
{
	tag = -1;
	isActive = active;
	transform = Transform::Transform(pos,_scale,ori);
	sprite = spr;
}

void GameObject::Draw()
{
	
	SetRenderMode(CDT_TEXTURE, 1.0f);
	SetTexture(sprite->GetTexture(), 0.0f, 0.0f);
	SetTransform(transform.GetModelMat());

	CDTMesh mesh = sprite->GetMesh();
	DrawMesh(mesh);

}

bool GameObject::CheckCollision(GameObject* onjToCheck)
{
	float rightA = transform.position.x + transform.scale.x / 2;
	float leftA = transform.position.x - transform.scale.x / 2;
	float topA = transform.position.y + transform.scale.y / 2;
	float bottomA = transform.position.y - transform.scale.y / 2;

	
	float rightB = onjToCheck->transform.position.x + onjToCheck->transform.scale.x / 2;
	float leftB = onjToCheck->transform.position.x - onjToCheck->transform.scale.x / 2;
	float topB = onjToCheck->transform.position.y + onjToCheck->transform.scale.y / 2;
	float bottomB = onjToCheck->transform.position.y - onjToCheck->transform.scale.y / 2;

	//+ Check for collsion
	return !(leftA > rightB || leftB > rightA || bottomA > topB || bottomB > topA);
}
