#include "imageloader.h"
#include "error.h"
#include <assert.h>

struct ImageLoader* imageloader_ctor(SDL_Renderer* renderer)
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
		debug(IMG_GetError(), ERRORTYPE_CRITICAL);
	struct ImageLoader* self = malloc(sizeof(struct ImageLoader));
	if(!self)
		debug("malloc", ERRORTYPE_MEMALLOC);

	self->renderer = renderer;
	return self;
}

struct Texture imageloader_load(struct ImageLoader* self, const char* file)
{
	assert(self);
	assert(file);

	struct Texture image;
	image.raw = IMG_LoadTexture(self->renderer, file);
	if(!image.raw)
		debug(IMG_GetError(), ERRORTYPE_APPLICATION);
	SDL_QueryTexture(
		image.raw, 
		NULL, 
		NULL, 
		&image.width, 
		&image.height
	);

	return image;
}

void imageloader_dtor(struct ImageLoader* self)
{
	assert(self);
	free(self);
	IMG_Quit();
}
