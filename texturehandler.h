#ifndef TEXTUREHANDLER_H
#define TEXTUREHANDLER_H

#include "texture.h"
#include "vec.h"

typedef size_t TextureID;

struct TextureHandler
{
	SDL_Renderer* renderer;
	Vec(struct Texture) textures;
};

struct TextureHandler* texturehandler_ctor(SDL_Renderer* renderer);
TextureID texturehandler_copy(
	struct TextureHandler* self,
	TextureID texture
);
TextureID texturehandler_add(
	struct TextureHandler* self, 
	struct Texture texture
);
//void texturehandler_render(struct TextureHandler* self, TextureID texture);
void texturehandler_dtor(struct TextureHandler* self);

#endif
