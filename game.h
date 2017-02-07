#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "scene.h"

enum FPSLimit
{
	FPSLIMIT_UNLIMITED = -1,
};

struct GameLoop
{
	enum FPSLimit fpslimit;
	int ticks;
};

struct Game
{
	Vec(struct Scene*) scenes;
	struct Window* window;
	struct GameLoop loop;
	size_t selectedscene;
	int done;
};

struct Game* game_ctor(struct GameLoop loop, struct Window* window);
void game_add(struct Game* self, struct Scene* scene);
void game_start(struct Game* self, void* userdata);
void game_dtor(struct Game* self);

#endif
