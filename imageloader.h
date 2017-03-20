#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <SDL2/SDL_image.h>
#include "texture.h"

struct ImageLoader
{
	SDL_Renderer* renderer;
};

struct ImageLoader* imageloader_ctor(
	struct ImageLoader* self, 
	SDL_Renderer* renderer
);
void imageloader_load(
	struct ImageLoader* self, 
	struct Texture* image, 
	const char* file
);
void imageloader_dtor(struct ImageLoader* self);

#endif
