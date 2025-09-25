#pragma once

#include "GameObject.h"
#include <SDL.h>
#include <cmath>

class Powerup : public GameObject {
private:
    bool collected;
    float direction;
    SDL_Rect collectRect;

public:
    Powerup(float x, float y, int w, int h);
    void update(int& count);
    void render(SDL_Renderer* renderer);
    void collectTo(SDL_Rect rect);
    bool isCollected() { return collected; }

};