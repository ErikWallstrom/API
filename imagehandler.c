#include "imagehandler.h"
#include "error.h"

#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <assert.h>

struct ImageHandler* imagehandler_ctor(SDL_Renderer* renderer)
{
	assert(renderer);

	const SDL_version *link_version = IMG_Linked_Version();
	SDL_version compile_version;
	SDL_IMAGE_VERSION(&compile_version);
	if(compile_version.major != link_version->major ||
		compile_version.minor != link_version->minor ||
		compile_version.patch != link_version->patch)
	{
		printf(
			"Warning: Program was compiled with SDL_image "
			"version %i.%i.%i, but was linked with version %i.%i.%i\n",
			compile_version.major,
			compile_version.minor,
			compile_version.patch,
			link_version->major,
			link_version->minor,
			link_version->patch
		);
	}

	if(!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
		debug(SDL_GetError(), ERRORTYPE_CRITICAL);
	struct ImageHandler* self = malloc(sizeof(struct ImageHandler));
	if(!self)
		debug("malloc", ERRORTYPE_MEMALLOC);

	self->renderer = renderer;
	self->images = vec_ctor(sizeof(struct Image), 0);
	return self;
}

ImageID imagehandler_copy(struct ImageHandler* self, ImageID img)
{
	assert(self);
	assert(img < vec_getsize(&self->images));

	Uint32 format;
	SDL_QueryTexture(
		self->images[img].texture, 
		&format, 
		NULL, 
		NULL, 
		NULL
	);

	struct Image image;
	image.width = self->images[img].width;
	image.height = self->images[img].height;
	image.texture = SDL_CreateTexture(
		self->renderer, 
		format, 
		SDL_TEXTUREACCESS_TARGET,
		image.width,
		image.height
	);
	if(!image.texture)
		debug(SDL_GetError(), ERRORTYPE_APPLICATION);

	SDL_SetTextureBlendMode(image.texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(self->renderer, image.texture);
	SDL_RenderCopy(
		self->renderer, 
		self->images[img].texture,
		&(SDL_Rect){0, 0, image.width, image.height},
		&(SDL_Rect){0, 0, image.width, image.height}
	);
	SDL_SetRenderTarget(self->renderer, NULL);

	vec_pushback(&self->images, image);
	return vec_getsize(&self->images) - 1;
}

ImageID imagehandler_load(struct ImageHandler* self, const char* file)
{
	assert(self);
	assert(file);

	struct Image image;
	image.texture = IMG_LoadTexture(self->renderer, file);
	if(!image.texture)
		debug(IMG_GetError(), ERRORTYPE_APPLICATION);
	SDL_QueryTexture(
		image.texture, 
		NULL, 
		NULL, 
		(int*)&image.width, 
		(int*)&image.height
	);

	vec_pushback(&self->images, image);
	return vec_getsize(&self->images) - 1;
}

void imagehandler_dtor(struct ImageHandler* self)
{
	assert(self);
	for(size_t i = 0; i < vec_getsize(&self->images); i++)
	{
		SDL_DestroyTexture(self->images[i].texture);
	}

	free(self);
	IMG_Quit();
}

