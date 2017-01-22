#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "scene.h"

enum FPSLimit
{
	FPSLIMIT_UNLIMITED = -2,
	FPSLIMIT_VSYNC = -1,
};

struct GameLoop
{
	enum FPSLimit fpslimit;
	int ticks;
};

struct Game
{
	struct Window* window;
	Vec(struct Scene*) scenes;
	struct GameLoop loop;
	size_t selectedscene;
	int done;
};

struct Game* game_ctor(int width, int height, struct GameLoop loop);
void game_add(struct Game* self, struct Scene* scene);
void game_start(struct Game* self, void* userdata);
void game_dtor(struct Game* self);

#endif
