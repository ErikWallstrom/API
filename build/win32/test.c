#include "../window.h"
#include "../game.h"
#include "../log.h"
#include <SDL2/SDL.h>

#undef main

struct Window window;

void update(struct Scene* scene, struct Game* game, void* userdata)
{
	static int ticks = 0;
	static Uint32 oldtime = 0;

	if(SDL_GetTicks() / 1000 > oldtime)
	{
		oldtime = SDL_GetTicks() / 1000;
		log_info("FPS: %i, Ticks: %i", window.fps, ticks);
		ticks = 0;
	}

	ticks++;
}

void render(struct Game* game, double interpolation, void* userdata)
{
	if(!window_update(&window))
	{
		game->done = 1;
	}
}

int main(void)
{
	window_ctor(&window, "Hello", 800, 600, WINDOW_RENDERER | WINDOW_VSYNC);
	struct Game game;
	game_ctor(&game, 60, NULL);
	game_add(&game, &(struct Scene){
		.update = update,
		.render = render
	});
	
	game_start(&game);
	game_dtor(&game);
	window_dtor(&window);
}
