#ifndef GAME_H
#define GAME_H

#include "scene.h"
#include "vec.h"

struct Game
{
	Vec(struct Scene*) scenes;
	void* userdata;
	size_t selectedscene;
	int ticks;
	int done;
};

struct Game* game_ctor(struct Game* self, int ticks, void* userdata);
void game_add(struct Game* self, struct Scene* scene);
void game_start(struct Game* self);
void game_dtor(struct Game* self);

#endif
