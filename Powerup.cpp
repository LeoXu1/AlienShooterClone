#include <iostream>
#include "Powerup.h"
#include "space_invaders_common.h"

Powerup::Powerup(float x, float y, int w, int h)
    : GameObject(x, y, w, h), collected(false), direction(0) {}

void Powerup::update(int& count) {
    if (collected) {
        x += std::cos(direction) * 10;
        y += std::sin(direction) * 10;
        if (x < collectRect.x + collectRect.w && y < collectRect.y + collectRect.h) {
            active = false;
            count++;
        }
        return;
    }
    else {
        y += 2;
    }
    if (y > SCREEN_HEIGHT) {
        active = false;
    }
}

void Powerup::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect powerupRect = getRect();
    SDL_RenderFillRect(renderer, &powerupRect);
}

void Powerup::collectTo(SDL_Rect rect) {
    direction = std::atan2(rect.y - y, rect.x - x);
    collectRect = rect;
    collected = true;
}