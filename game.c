#include "game.h"
#include "log.h"
#include <inttypes.h>

#ifdef __WIN32__
#include <Windows.h>
#define BILLION                             (1E9)
#define TIME_UTC 1

struct timespec { long tv_sec; long tv_nsec; }; 
int clock_gettime(int dummy, struct timespec *ct)
{
	(void)dummy;
	static BOOL g_first_time = 1;
	static LARGE_INTEGER g_counts_per_sec;
	LARGE_INTEGER count;

	if (g_first_time)
	{
		g_first_time = 0;

		if (0 == QueryPerformanceFrequency(&g_counts_per_sec))
		{
		    g_counts_per_sec.QuadPart = 0;
		}
	}

	if ((NULL == ct) || (g_counts_per_sec.QuadPart <= 0) ||
	    (0 == QueryPerformanceCounter(&count)))
	{
		return -1;
	}

	ct->tv_sec = count.QuadPart / g_counts_per_sec.QuadPart;
	ct->tv_nsec = ((count.QuadPart % g_counts_per_sec.QuadPart) * BILLION) / g_counts_per_sec.QuadPart;

	return 0;
}

void timespec_get(struct timespec* ts, int dummy)
{
	clock_gettime(dummy, ts);
}

#else
#include <time.h>
#endif

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
