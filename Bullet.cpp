#include "Bullet.h"

Bullet::Bullet(float x, float y, int w, int h, bool friendly)
    : GameObject(x, y, w, h), friendly(friendly) {}