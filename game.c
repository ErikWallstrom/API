#include "game.h"
#include "error.h"
#include <time.h>
#include <assert.h>

struct Game* game_ctor(struct GameLoop loop)
{
	assert(loop.ticks > 0);
	assert(loop.fpslimit >= FPSLIMIT_UNLIMITED);

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

	self->scenes = vec_ctor(sizeof(struct Scene*), 0);
	self->window = NULL;
	self->loop = loop;
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

	double oldtime = SDL_GetPerformanceCounter() * 1000.0;
	double lag = 0.0;
	double sleeptime = 0.0;
	double sleeptick = SDL_GetTicks();

	while(!self->done)
	{
		double msperupdate = 1000.0 / (double)self->loop.ticks;
		double curtime = SDL_GetPerformanceCounter() * 1000.0;
		double delta = (curtime - oldtime) / SDL_GetPerformanceFrequency();
		oldtime = curtime;
		lag += delta;

		if(self->window)
			self->done = !window_update(self->window);
		if(vec_getsize(&self->scenes))
		{
			struct Scene* scene = self->scenes[self->selectedscene];
			if(scene->update)
			{
				while(lag >= msperupdate)
				{
					scene->update(scene, self, user_data);
					lag -= msperupdate;

					if(self->window)
						self->window->read = 1;
				}
			}
			if(scene->render)
			{
				double interpolation = lag / msperupdate;
				scene->render(
					self,
					interpolation,
					user_data
				);
			}

			switch(scene->change)
			{
			case SCENECHANGE_NEXT:
				assert(self->selectedscene + 1 < vec_getsize(&self->scenes));
				self->scenes[self->selectedscene]->change = SCENECHANGE_NONE;
				self->selectedscene++;
				break;
			case SCENECHANGE_PREV:
				assert(self->selectedscene > 0);
				self->scenes[self->selectedscene]->change = SCENECHANGE_NONE;
				self->selectedscene--;
				break;
			default:;
			}
		}

		if(self->loop.fpslimit > 0)
		{
			sleeptick += 1000.0 / self->loop.fpslimit;
			sleeptime = sleeptick - SDL_GetTicks();

			if(sleeptime >= 0)
				SDL_Delay(sleeptime);
		}
	}
}

void game_dtor(struct Game* self)
{
	assert(self);
	vec_dtor(&self->scenes);

	free(self);
	SDL_Quit();
}
