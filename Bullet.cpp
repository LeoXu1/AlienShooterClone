#include "Bullet.h"

Bullet::Bullet(float x, float y, int w, int h, bool friendly)
    : GameObject(x, y, w, h), friendly(friendly) {
        if (friendly) {
            direction = -M_PI / 2;
        } else {
            direction = M_PI / 2;
        }
    }

Bullet::Bullet(float x, float y, int w, int h, bool friendly, float direction)
    : GameObject(x, y, w, h), friendly(friendly), direction(direction * M_PI / 180)  {}

void Bullet::update() {
    x += std::cos(direction) * BULLET_SPEED;
    y += std::sin(direction) * BULLET_SPEED;

    if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT) {
        active = false;
    }
}

void Bullet::render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 255, friendly ? 255 : 0, 0, 255);
    SDL_Rect bulletRect = getRect();
    SDL_RenderFillRect(renderer, &bulletRect);
}
