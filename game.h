#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "scene.h"

struct Game
{
	struct Window* window;
	Vec(struct Scene*) scenes;
	size_t selectedscene;
	float delta;
	int done;
};

struct Game* game_ctor(int width, int height);
void game_add(struct Game* self, struct Scene* scene);
void game_start(struct Game* self, void* userdata);
void game_dtor(struct Game* self);

#endif
