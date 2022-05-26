#include "AnimatedObject.h"

AnimatedObject::AnimatedObject(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr):GameObject(active,pos,scale,ori,spr)
{
	frame = 0;
	curFrame = 0;
	numFrame = 0;
	offsetX = 0.0f;
	offsetY = 0.0f;
}

void AnimatedObject::Draw()
{
	SetRenderMode(CDT_TEXTURE, 1.0f);
	SetTexture(sprite->GetTexture(), offsetX, offsetY);
	SetTransform(transform.GetModelMat());

	CDTMesh mesh = sprite->GetMesh();
	DrawMesh(mesh);
}
