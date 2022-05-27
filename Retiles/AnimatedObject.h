#pragma once
#include "GameObject.h"
#define ANIM_SPEED 24
class AnimatedObject :
    public GameObject
{
protected:
    int frame; 
    int curFrame;
    int numFrame;
public:
    float offsetX;
    float offsetY;

    AnimatedObject(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr);
    virtual void UpdateAnimation() { return; }
    void Draw() override;
};

