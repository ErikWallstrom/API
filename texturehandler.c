#include "texturehandler.h"
#include "error.h"

#include <stdlib.h>
#include <assert.h>

struct TextureHandler* texturehandler_ctor(SDL_Renderer* renderer)
{
	assert(renderer);
	struct TextureHandler* self = malloc(
		sizeof(struct TextureHandler)
	);
	if(!self)
		debug("malloc", ERRORTYPE_MEMALLOC);
	self->textures = vec_ctor(sizeof(struct Texture), 0);
	self->renderer = renderer;
	return self;
}

TextureID texturehandler_copy(
	struct TextureHandler* self,
	TextureID texture
)
{
	assert(self);
	assert(texture < vec_getsize(&self->textures));

	Uint32 format;
	SDL_QueryTexture(
		self->textures[texture].raw, 
		&format, 
		NULL, 
		NULL, 
		NULL
	);

	struct Texture tex;
	tex.width = self->textures[texture].width;
	tex.height = self->textures[texture].height;
	tex.raw = SDL_CreateTexture(
		self->renderer, 
		format, 
		SDL_TEXTUREACCESS_TARGET,
		tex.width,
		tex.height
	);
	if(!tex.raw)
		debug(SDL_GetError(), ERRORTYPE_APPLICATION);

	SDL_SetTextureBlendMode(tex.raw, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(self->renderer, tex.raw);
	SDL_RenderCopy(
		self->renderer, 
		self->textures[texture].raw,
		&(SDL_Rect){0, 0, tex.width, tex.height},
		&(SDL_Rect){0, 0, tex.width, tex.height}
	);
	SDL_SetRenderTarget(self->renderer, NULL);

	vec_pushback(&self->textures, tex);
	return vec_getsize(&self->textures) - 1;
}

TextureID texturehandler_add(
	struct TextureHandler* self, 
	struct Texture texture
)
{
	assert(self);
	assert(texture.raw);

	vec_pushback(&self->textures, texture);
	return vec_getsize(&self->textures) - 1;
}

void texturehandler_dtor(struct TextureHandler* self)
{
	assert(self);
	for(size_t i = 0; i < vec_getsize(&self->textures); i++)
		SDL_DestroyTexture(self->textures[i].raw);
	free(self);
}

