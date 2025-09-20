#include "GameObject.h"

SDL_Rect GameObject::getRect() const {
    return {(int)x, (int)y, width, height};
}

bool GameObject::collidesWith(const GameObject& other) const {
    return active && other.active &&
           x < other.x + other.width &&
           x + width > other.x &&
           y < other.y + other.height &&
           y + height > other.y;
}