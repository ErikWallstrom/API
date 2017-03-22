#include "fontloader.h"
#include "log.h"

struct FontLoader* fontloader_ctor(
	struct FontLoader* self, 
	SDL_Renderer* renderer
)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");

	const SDL_version *link_version = TTF_Linked_Version();
	SDL_version compile_version;
	SDL_TTF_VERSION(&compile_version);
	if(compile_version.major != link_version->major ||
		compile_version.minor != link_version->minor ||
		compile_version.patch != link_version->patch)
	{
		log_warning(
			"Program was compiled with SDL_ttf "
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
		log_error(TTF_GetError());

	self->renderer = renderer;
	self->fonts	= vec_ctor(struct Font, 0);
	return self;
}

FontID fontloader_load(struct FontLoader* self, const char* file, size_t size)
{
	log_assert(self, "is NULL");
	log_assert(file, "is NULL");
	log_assert(size, "is 0");

	TTF_Font* font = TTF_OpenFont(file, size);
	if(!font)
		log_error(TTF_GetError());

	vec_pushback(&self->fonts, (struct Font){
		.raw = font,
		.size = size
	});
	return vec_getsize(&self->fonts) - 1;
}

void fontloader_render(
	struct FontLoader* self, 
	struct Texture* texture,
	const char* text,
	FontID font, 
	enum FontQuality quality
)
{
	log_assert(self, "is NULL");
	log_assert(texture, "is NULL");
	log_assert(text, "is NULL");
	log_assert(font < vec_getsize(&self->fonts), "invalid FontID");
	log_assert(
		quality == FONTQUALITY_LOW || 
			quality == FONTQUALITY_HIGH, 
		"is NULL"
	);

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
		log_error("Invalid font quality");
	}

	texture->raw = SDL_CreateTextureFromSurface(
		self->renderer,
		surface
	);
	if(!texture->raw)
		log_error(SDL_GetError());

	SDL_FreeSurface(surface);
	SDL_QueryTexture(
		texture->raw, 
		NULL, 
		NULL, 
		&texture->width,
		&texture->height
	);
}

void fontloader_dtor(struct FontLoader* self)
{
	log_assert(self, "is NULL");
	for(size_t i = 0; i < vec_getsize(&self->fonts); i++)
		TTF_CloseFont(self->fonts[i].raw);

	vec_dtor(&self->fonts);
	TTF_Quit();
}
