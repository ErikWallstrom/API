#include "game.h"
#include "error.h"
#include <time.h>
#include <assert.h>

struct Game* game_ctor(int width, int height)
{
	assert(width > 0);
	assert(height > 0);

	SDL_version compile_version;
	SDL_version link_version;

	SDL_VERSION(&compile_version);
	SDL_GetVersion(&link_version);
	if(compile_version.major != link_version.major ||
		compile_version.minor != link_version.minor ||
		compile_version.patch != link_version.patch)
	{
		printf(
			"Warning: Program was compiled with SDL version %i.%i.%i,"
			" but was linked with version %i.%i.%i\n",
			compile_version.major,
			compile_version.minor,
			compile_version.patch,
			link_version.major,
			link_version.minor,
			link_version.patch
		);
	}

	struct Game* self = malloc(sizeof(struct Game));
	if(!self)
		debug("malloc", ERRORTYPE_MEMALLOC);

	if(SDL_Init(SDL_INIT_VIDEO))
		debug(SDL_GetError(), ERRORTYPE_CRITICAL);

	srand(time(NULL));
	self->window = window_ctor(
		"Game Window", 
		width, 
		height, 
		WINDOW_RENDERER
	);
	self->scenes = vec_ctor(sizeof(struct Scene*), 0);
	self->selectedscene = 0;
	self->done = 0;
	return self;
}

void game_add(struct Game* self, struct Scene* scene)
{
	assert(self);
	assert(scene);

	vec_pushback(&self->scenes, scene);
}

void game_start(struct Game* self, void* user_data)
{
	assert(self);

	Uint64 oldtime = SDL_GetPerformanceCounter();
	while(!self->done)
	{
		self->delta = ((SDL_GetPerformanceCounter() - oldtime) * 1000.f) /
			SDL_GetPerformanceFrequency();
		oldtime = SDL_GetPerformanceCounter();

		self->done = !window_update(self->window);
		if(vec_getsize(&self->scenes))
		{
			struct Scene* scene = self->scenes[self->selectedscene];
			if(scene->update)
				scene->update(scene, self, user_data);

			switch(scene->change)
			{
			case SCENE_CHANGE_NEXT:
				assert(self->selectedscene + 1 < vec_getsize(&self->scenes));
				self->selectedscene++;
				break;
			case SCENE_CHANGE_PREV:
				assert(self->selectedscene > 0);
				self->selectedscene--;
				break;
			default:;
			}
		}

	}
}

void game_dtor(struct Game* self)
{
	assert(self);
	window_dtor(self->window);
	vec_dtor(&self->scenes);

	free(self);
	SDL_Quit();
}
