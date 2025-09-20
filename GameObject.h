#pragma once

#include <SDL.h>

struct GameObject {
    float x, y;
    int width, height;
    bool active;

    GameObject(float x = 0, float y = 0, int w = 0, int h = 0)
        : x(x), y(y), width(w), height(h), active(true) {}

    SDL_Rect getRect() const;
    bool collidesWith(const GameObject& other) const;
};