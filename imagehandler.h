#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include <SDL2/SDL_image.h>
#include "vec.h"

typedef size_t ImageID;
struct Image
{
	SDL_Texture* texture;
	size_t width, height;
};

struct ImageHandler
{
	SDL_Renderer* renderer;
	Vec(struct Image) images;
};

struct ImageHandler* imagehandler_ctor(SDL_Renderer* renderer);
ImageID imagehandler_copy(struct ImageHandler* self, ImageID img);
ImageID imagehandler_load(struct ImageHandler* self, const char* file);
void imagehandler_dtor(struct ImageHandler* self);

#endif
