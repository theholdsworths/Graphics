#include "Sprite.h"

Sprite::Sprite() : Sprite(0, 0, 10, 10) {}

Sprite::Sprite(int x, int y, int w, int h)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite Constructed (%p)", this);
    rectangle.x = x;
    rectangle.y = y;
    rectangle.w = w;
    rectangle.h = h;
}

Sprite::Sprite(const Sprite& sprite) : rectangle(sprite.rectangle)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite Constructed (%p) - Copied from (%p)", this, &sprite);
}

Sprite::~Sprite()
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite Destructed  (%p)", this);
}
