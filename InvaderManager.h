#pragma once

#include "GameObject.h"
#include "Bullet.h"
#include "Powerup.h"
#include <vector>
#include <SDL.h>

class InvaderManager {
private:
    std::vector<GameObject> invaders;
    int invaderDirection;
    Uint32 lastInvaderMove;
    Uint32 lastInvaderShot;

public:
    InvaderManager();
    void initializeInvaders();
    std::vector<GameObject>& getInvaders();
    void clearInvaders();
    void update(std::vector<Bullet>& invaderBullets);
    void render(SDL_Renderer* renderer);
    void checkCollisions(std::vector<Bullet>& playerBullets, std::vector<Powerup>& powerUps);
    bool allInvadersDestroyed();
};