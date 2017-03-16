#include "texture.h"
#include "log.h"

struct Texture* texture_copy(
	struct Texture* dest, 
	struct Texture* from, 
	SDL_Renderer* renderer
)
{
	log_assert(dest, "is NULL");
	log_assert(from, "is NULL");

	Uint32 format;
	SDL_QueryTexture(
		from->raw,
		&format, 
		NULL, 
		NULL, 
		NULL
	);

	dest->width = from->width;
	dest->height = from->height;
	dest->raw = SDL_CreateTexture(
		renderer, 
		format, 
		SDL_TEXTUREACCESS_TARGET,
		dest->width,
		dest->height
	);
	if(!dest->raw)
		log_error(SDL_GetError());

	SDL_SetTextureBlendMode(dest->raw, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, dest->raw);
	SDL_RenderCopy(
		renderer, 
		from->raw,
		&(SDL_Rect){0, 0, dest->width, dest->height},
		&(SDL_Rect){0, 0, dest->width, dest->height}
	);
	SDL_SetRenderTarget(renderer, NULL);
	return dest;
}

