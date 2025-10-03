#pragma once

#include "GameObject.h"
#include <SDL.h>

class Bullet : public GameObject {
private:
    bool friendly;
    float direction;
public:
    Bullet(float x, float y, int w, int h, bool friendly);
    void update();
    void render(SDL_Renderer* renderer);
    
};