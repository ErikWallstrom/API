#include "../dobjecthandler.h"
#include "../imageloader.h"
#include "../window.h"
#include "../game.h"
#include "../log.h"

struct DObject obj1, obj2;

void movebehavior(struct DObject* self, struct Scene* scene, void* userdata)
{
	dobject_defaultbehavior(self, scene, userdata);
	struct Window* window = scene->userdata;
	if(window->keystate[SDL_SCANCODE_W])
	{
		self->changey--;
	}
	if(window->keystate[SDL_SCANCODE_S])
	{
		self->changey++;
	}
	if(window->keystate[SDL_SCANCODE_A])
	{
		self->changex--;
	}
	if(window->keystate[SDL_SCANCODE_D])
	{
		self->changex++;
	}

	if(dobject_intersects(&obj2, &obj1))
	{
		log_info("INTERSECTING OBJ, %i", rand());
	}

	if(dobject_hittestleft(&obj2, &obj1))
	{
		log_info("HIT LEFT");
	}

	if(dobject_hittestright(&obj2, &obj1))
	{
		log_info("HIT RIGHT");
	}

	if(dobject_hittesttop(&obj2, &obj1))
	{
		log_info("HIT TOP");
	}

	if(dobject_hittestbottom(&obj2, &obj1))
	{
		log_info("HIT BOTTOM");
	}
}

void update(struct Scene* scene, struct Game* game)
{
	struct DObjectHandler* handler = game->userdata;
	dobjecthandler_update(handler);
}

void render(struct Scene* scene, struct Game* game, double interpolation)
{
	struct DObjectHandler* handler = game->userdata;
	dobjecthandler_render(handler, interpolation);

	struct Window* window = scene->userdata;
	game->done = !window_update(window);
}

int main(void)
{
	struct Game game;
	game_ctor(&game, 60, NULL);

	struct Window window;
	window_ctor(
		&window, 
		"Game Window", 
		800, 
		600, 
		WINDOW_RENDERER | WINDOW_VSYNC
	);

	struct Scene scene;
	scene_ctor(&scene, update, render, &window);
	game_add(&game, &scene);

	struct DObjectHandler handler;
	dobjecthandler_ctor(&handler, window.renderer, &scene, NULL);
	game.userdata = &handler;

	struct ImageLoader loader;
	imageloader_ctor(&loader, window.renderer);

	struct Texture texture;
	imageloader_load(&loader, &texture, "./Test.jpg");

	dobjecthandler_add(&handler, &obj1);
	dobjecthandler_add(&handler, &obj2);

	dobject_ctor(
		&obj1, 
		&texture, 
		800, 
		600, 
		DOBJECTREG_BOTTOMRIGHT, 
		movebehavior,
		dobject_defaultrender,
		NULL
	);

	dobject_ctor(
		&obj2, 
		&texture, 
		400, 
		300, 
		DOBJECTREG_CENTER, 
		dobject_defaultbehavior,
		dobject_defaultrender,
		NULL
	);

	obj1.w = 200;
	obj1.h = 200;
	obj2.w = 200;
	obj2.h = 200;

	game_start(&game);
	dobjecthandler_dtor(&handler);
	imageloader_dtor(&loader);
	window_dtor(&window);
	game_dtor(&game);
}

