#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>

struct Texture
{
	SDL_Texture* raw;
	size_t width, height;
};

#endif
