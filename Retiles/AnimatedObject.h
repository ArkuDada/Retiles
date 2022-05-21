#pragma once
#include "GameObject.h"
class AnimatedObject :
    public GameObject
{
protected:
    int curFrame;
    int numFrame;
    float offsetX;
    float offsetY;

public:
    AnimatedObject(bool active, glm::vec3 pos, glm::vec3 scale, float ori, Sprite* spr);
    virtual void UpdateAnimation() = 0;
    void Draw() override;
};

