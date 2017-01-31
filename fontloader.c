#include "fontloader.h"
#include "error.h"
#include <assert.h>

struct FontLoader* fontloader_ctor(SDL_Renderer* renderer)
{
	assert(renderer);

	const SDL_version *link_version = TTF_Linked_Version();
	SDL_version compile_version;
	SDL_TTF_VERSION(&compile_version);
	if(compile_version.major != link_version->major ||
		compile_version.minor != link_version->minor ||
		compile_version.patch != link_version->patch)
	{
		printf(
			"Warning: Program was compiled with SDL_ttf "
			"version %i.%i.%i, but was linked with version %i.%i.%i\n",
			compile_version.major,
			compile_version.minor,
			compile_version.patch,
			link_version->major,
			link_version->minor,
			link_version->patch
		);
	}

	if(TTF_Init())
		debug(TTF_GetError(), ERRORTYPE_CRITICAL);

	struct FontLoader* self = malloc(sizeof(struct FontLoader));
	if(!self)
		debug("malloc", ERRORTYPE_MEMALLOC);

	self->renderer = renderer;
	self->fonts	= vec_ctor(sizeof(struct Font), 0);
	return self;
}

FontID fontloader_load(struct FontLoader* self, const char* file, size_t size)
{
	assert(self);
	assert(file);
	assert(size);

	TTF_Font* font = TTF_OpenFont(file, size);
	if(!font)
		debug(TTF_GetError(), ERRORTYPE_APPLICATION);

	vec_pushback(&self->fonts, (struct Font){
		.raw = font,
		.size = size
	});
	return vec_getsize(&self->fonts) - 1;
}

struct Texture fontloader_render(
	struct FontLoader* self, 
	const char* text,
	FontID font, 
	enum FontQuality quality
)
{
	assert(self);
	SDL_Surface* surface = NULL;
	switch(quality)
	{
	case FONTQUALITY_HIGH:
		surface = TTF_RenderText_Blended(
			self->fonts[font].raw, 
			text, 
			(SDL_Color){255, 255, 255, 255}
		);
		break;

	case FONTQUALITY_LOW:
		surface = TTF_RenderText_Solid(
			self->fonts[font].raw, 
			text, 
			(SDL_Color){255, 255, 255, 255}
		);
		break;
	default:
		debug("Invalid font quality", ERRORTYPE_APPLICATION);
	}

	struct Texture texture;
	texture.raw = SDL_CreateTextureFromSurface(
		self->renderer,
		surface
	);
	if(!texture.raw)
		debug(SDL_GetError(), ERRORTYPE_APPLICATION);

	SDL_FreeSurface(surface);
	SDL_QueryTexture(
		texture.raw, 
		NULL, 
		NULL, 
		(int*)&texture.width, 
		(int*)&texture.height
	);

	return texture;
}

void fontloader_dtor(struct FontLoader* self)
{
	assert(self);
	for(size_t i = 0; i < vec_getsize(&self->fonts); i++)
		TTF_CloseFont(self->fonts[i].raw);

	vec_dtor(&self->fonts);
	free(self);
	TTF_Quit();
}
