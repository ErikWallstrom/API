#include "game.h"
#include <SDL2/SDL.h>

struct Rectangle
{
	double x, y;
	int w, h;
	double speed;
	double changex, changey;
};

void update(struct Scene* scene, struct Window* window, void* _)
{
	struct Rectangle* player = _;

	player->x += player->changex;
	player->y += player->changey;
	player->changex = 0.0;
	player->changey = 0.0;

	if(window->keystate[SDL_SCANCODE_W])
	{
		player->changey -= player->speed;
	}
	if(window->keystate[SDL_SCANCODE_S])
	{
		player->changey += player->speed;
	}
	if(window->keystate[SDL_SCANCODE_A])
	{
		player->changex -= player->speed;
	}
	if(window->keystate[SDL_SCANCODE_D])
	{
		player->changex += player->speed;
	}

	static Uint32 oldticks = 0;
	static int frames = 0;
	if(SDL_GetTicks() / 1000 > oldticks)
	{
		oldticks = SDL_GetTicks() / 1000;
		printf("Ticks: %i, FPS: %i\n", frames, window->fps);
		frames = 0;
	}
	frames++;
}

void render(SDL_Renderer* renderer, double interpolation, void* _)
{
	struct Rectangle* player = _;

	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	SDL_RenderFillRect(renderer, &(SDL_Rect){
		player->x + (player->changex * interpolation),
		player->y + (player->changey * interpolation),
		player->w,
		player->h
	});

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

int main(void)
{
	struct Rectangle player = {
		.x = 50.0, .y = 50.0, 
		.w = 50, .h = 50, 
		.speed = 5.0, 
		.changex = 0.0, .changey = 0.0
	};
	
	struct Game* game = game_ctor(
		800, 600, 
		(struct GameLoop){
			.fpslimit = FPSLIMIT_VSYNC,
			.ticks = 60
		}
	);

	game_add(
		game,
		&(struct Scene){
			.change = SCENECHANGE_NONE,
			.update = update,
			.render = render
		}
	);

	game_start(game, &player);
	game_dtor(game);
}
