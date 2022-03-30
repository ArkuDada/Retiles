#include "GameObject.h"

GameObject::GameObject(CDTMesh* mesh, CDTTex* tex, int type, int flag, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient)
{
	this->mesh = mesh;
	this->tex = tex;
	this->type = type;
	this->flag = flag;
	this->position = pos;
	this->velocity = vel;
	this->scale = scale;
	this->orientation = orient;
	this->modelMatrix = glm::mat4(1.0f);
	
}
