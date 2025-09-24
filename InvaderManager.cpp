#include "InvaderManager.h"
#include "space_invaders_common.h"
#include <cstdlib>

InvaderManager::InvaderManager()
    : invaderDirection(1), lastInvaderMove(0), lastInvaderShot(0) {
    //initializeInvaders();
}

void InvaderManager::initializeInvaders() {
    invaders.clear();
    for (int row = 0; row < INVADER_ROWS; row++) {
        for (int col = 0; col < INVADER_COLS; col++) {
            float x = 50 + col * 60;
            float y = 100 + row * 50;
            invaders.push_back(GameObject(x, y, 40, 30));
        }
    }
}

std::vector<GameObject>& InvaderManager::getInvaders() {
    return invaders;
}

void InvaderManager::clearInvaders() {
    invaders.clear();
}

void InvaderManager::update(std::vector<GameObject>& invaderBullets) {
    Uint32 currentTime = SDL_GetTicks();

    // Move invaders
    if (currentTime - lastInvaderMove > 500) { // Move every 500ms
        bool changeDirection = false;

        for (const auto& invader : invaders) {
            if (!invader.active) continue;
            if ((invaderDirection > 0 && invader.x + invader.width >= SCREEN_WIDTH - 10) ||
                (invaderDirection < 0 && invader.x <= 10)) {
                changeDirection = true;
                break;
            }
        }

        if (changeDirection) {
            invaderDirection *= -1;
            // for (auto& invader : invaders) {
            //     if (invader.active) {
            //         invader.y += 20;
            //     }
            // }
        } else {
            for (auto& invader : invaders) {
                if (invader.active) {
                    invader.x += invaderDirection * INVADER_SPEED * 10;
                }
            }
        }
        lastInvaderMove = currentTime;
    }

    // Invader shooting
    if (currentTime - lastInvaderShot > 1000) { // Shoot every 1000ms
        std::vector<GameObject*> activeInvaders;
        for (auto& invader : invaders) {
            if (invader.active) {
                activeInvaders.push_back(&invader);
            }
        }

        if (!activeInvaders.empty()) {
            GameObject* shooter = activeInvaders[rand() % activeInvaders.size()];
            invaderBullets.push_back(GameObject(shooter->x + shooter->width/2 - 2,
                                             shooter->y + shooter->height, 4, 10));
        }
        lastInvaderShot = currentTime;
    }
}

void InvaderManager::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const auto& invader : invaders) {
        if (invader.active) {
            SDL_Rect invaderRect = invader.getRect();
            SDL_RenderFillRect(renderer, &invaderRect);
        }
    }
}

void InvaderManager::checkCollisions(std::vector<GameObject>& playerBullets, std::vector<Powerup>& powerUps) {
    for (auto& bullet : playerBullets) {
        if (!bullet.active) continue;
        for (auto& invader : invaders) {
            if (bullet.collidesWith(invader)) {
                bullet.active = false;
                invader.active = false;
                int r = rand() % 100;
                if (r < 20) powerUps.push_back(Powerup(invader.x, invader.y, 10, 10));
                break;
            }
        }
    }
}

bool InvaderManager::allInvadersDestroyed() {
    for (const auto& invader : invaders) {
        if (invader.active) {
            return false;
        }
    }
    return true;
}