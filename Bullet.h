#pragma once

#include "GameObject.h"
#include "space_invaders_common.h"
#include <SDL.h>
#include <cmath>

class Bullet : public GameObject {
private:
    bool friendly;
    float direction;
public:
    Bullet(float x, float y, int w, int h, bool friendly);
    Bullet(float x, float y, int w, int h, bool friendly, float direction);
    bool isFriendly() const { return friendly; }
    void update();
    void render(SDL_Renderer* renderer) const;
};