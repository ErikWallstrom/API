#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>

struct Texture
{
	SDL_Texture* raw;
	int width, height;
};

struct Texture* texture_copy(
	struct Texture* dest, 
	struct Texture* from, 
	SDL_Renderer* renderer
);

#endif
