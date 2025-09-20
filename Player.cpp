#include "Player.h"
#include "space_invaders_common.h"
#include "GameObject.h"

Player::Player(float x, float y, int w, int h)
    : GameObject(x, y, w, h), lastShot(0) {}

void Player::handleInput() {

    const Uint8* keyState = SDL_GetKeyboardState(nullptr);

    // Player movement
    if (keyState[SDL_SCANCODE_LEFT]) {
        x -= PLAYER_SPEED;
    }
    if (keyState[SDL_SCANCODE_RIGHT]) {
        x += PLAYER_SPEED;
    }
    if (keyState[SDL_SCANCODE_UP]) {
        y -= PLAYER_SPEED;
    }
    if (keyState[SDL_SCANCODE_DOWN]) {
        y += PLAYER_SPEED;
    }

}

void Player::update(std::vector<GameObject>& bullets) {
    // Clamp player position to screen bounds
    if (x < 0) x = 0;
    if (x > SCREEN_WIDTH - width) x = SCREEN_WIDTH - width;
    if (y < 0) y = 0;
    if (y > SCREEN_HEIGHT - height) y = SCREEN_HEIGHT - height;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShot > 250) { // Limit firing rate
        bullets.push_back(GameObject(x + width/2 - 2, y, 4, 10));
        lastShot = currentTime;
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect playerRect = getRect();
    SDL_RenderFillRect(renderer, &playerRect);
}

void Player::reset(float x, float y) {
    this->x = x;
    this->y = y;
}