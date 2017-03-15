#include "game.h"
#include "log.h"
#include <time.h>
#include <inttypes.h>

struct Game* game_ctor(struct Game* self, int ticks, void* userdata)
{
	log_assert(self, "is NULL");
	log_assert(ticks > 0, "is invalid");

	self->scenes = vec_ctor(struct Scene*, 0);
	self->userdata = userdata;
	self->ticks = ticks;
	self->selectedscene = 0;
	self->done = 0;
	return self;
}

void game_add(struct Game* self, struct Scene* scene)
{
	log_assert(self, "is NULL");
	log_assert(scene, "is NULL");

	vec_pushback(&self->scenes, scene);
}

int64_t game_getperformancefrequency(void)
{
	return 1000000000;
}

int64_t game_getperformancecounter(void)
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	uint64_t ticks = ts.tv_sec;
	ticks *= game_getperformancefrequency();
	ticks += ts.tv_nsec;

	return ticks;
}

void game_start(struct Game* self)
{
	log_assert(self, "is NULL");

	double oldtime = game_getperformancecounter() * 1000.0;
	double lag = 0.0;

	while(!self->done)
	{
		double msperupdate = 1000.0 / self->ticks;
		double curtime = game_getperformancecounter() * 1000.0;
		double delta = (curtime - oldtime) /
			game_getperformancefrequency();
		oldtime = curtime;
		lag += delta;

		if(vec_getsize(&self->scenes))
		{
			struct Scene* scene = self->scenes[self->selectedscene];
			if(scene->update)
			{
				while(lag >= msperupdate)
				{
					scene->update(scene, self, self->userdata);
					lag -= msperupdate;
				}
			}

			if(scene->render)
			{
				double interpolation = lag / msperupdate;
				scene->render(
					self,
					interpolation,
					self->userdata
				);
			}

			switch(scene->change)
			{
			case SCENECHANGE_NEXT:
				log_assert(
					self->selectedscene + 1 < vec_getsize(&self->scenes), 
					"scene does not exist"
				);
				self->scenes[self->selectedscene]->change = 
					SCENECHANGE_NONE;
				self->selectedscene++;
				break;
			case SCENECHANGE_PREV:
				log_assert(self->selectedscene > 0, "scene does not exist");
				self->scenes[self->selectedscene]->change = 
					SCENECHANGE_NONE;
				self->selectedscene--;
				break;
			default:;
			}
		}
	}
}

void game_dtor(struct Game* self)
{
	log_assert(self, "is NULL");
	vec_dtor(&self->scenes);
}
