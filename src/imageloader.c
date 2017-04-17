#include "imageloader.h"
#include "log.h"

struct ImageLoader* imageloader_ctor(
	struct ImageLoader* self, 
	SDL_Renderer* renderer
)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");

	const SDL_version *link_version = IMG_Linked_Version();
	SDL_version compile_version;
	SDL_IMAGE_VERSION(&compile_version);
	if(compile_version.major != link_version->major ||
		compile_version.minor != link_version->minor ||
		compile_version.patch != link_version->patch)
	{
		log_warning(
			"Program was compiled with SDL_image "
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
		log_error(IMG_GetError());

	self->renderer = renderer;
	return self;
}

void imageloader_load(
	struct ImageLoader* self, 
	struct Texture* image, 
	const char* file
)
{
	log_assert(self, "is NULL");
	log_assert(image, "is NULL");
	log_assert(file, "is NULL");

	image->raw = IMG_LoadTexture(self->renderer, file);
	if(!image->raw)
		log_error(IMG_GetError());
	SDL_QueryTexture(
		image->raw, 
		NULL, 
		NULL, 
		&image->width, 
		&image->height
	);
}

void imageloader_dtor(struct ImageLoader* self)
{
	log_assert(self, "is NULL");
	IMG_Quit();
}
