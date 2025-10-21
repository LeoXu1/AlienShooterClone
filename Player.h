#pragma once

#include "GameObject.h"
#include "Bullet.h"
#include <SDL.h>
#include <vector>

class Player : public GameObject {
private:
    Uint32 lastShot;

public:
    Player(float x, float y, int w, int h);

    void handleInput();
    void update(std::vector<Bullet>& bullets);
    void render(SDL_Renderer* renderer);
    void reset(float x, float y);
};