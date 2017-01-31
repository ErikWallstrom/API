#ifndef FONTLOADER_H
#define FONTLOADER_H

#include <SDL2/SDL_ttf.h>
#include "texture.h"
#include "vec.h"

enum FontQuality
{
	FONTQUALITY_LOW,
	FONTQUALITY_HIGH
};

struct Font
{
	TTF_Font* raw;
	size_t size;
};

typedef size_t FontID;
struct FontLoader
{
	SDL_Renderer* renderer;
	Vec(struct Font) fonts;
};

struct FontLoader* fontloader_ctor(SDL_Renderer* renderer);
FontID fontloader_load(struct FontLoader* self, const char* file, size_t size);
struct Texture fontloader_render(
	struct FontLoader* self, 
	const char* text,
	FontID font, 
	enum FontQuality quality
);
void fontloader_dtor(struct FontLoader* self);

#endif
