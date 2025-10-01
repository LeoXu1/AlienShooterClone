#pragma once

#include "GameObject.h"
#include <SDL.h>

class Bullet : public GameObject {
private:
    bool friendly;
public:
    Bullet(float x, float y, int w, int h);
};