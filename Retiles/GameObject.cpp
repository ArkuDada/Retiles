#include "GameObject.h"

GameObject::GameObject(bool active, glm::vec3 pos, glm::vec3 _scale, float ori, Sprite* spr)
{
	body = PhysicObject(&transform);

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

