#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include "str.h" 

enum WindowFlags
{
	WINDOW_VSYNC = 1 << 0,
	WINDOW_CONTEXT = 1 << 1,
	WINDOW_RENDERER = 1 << 2,
};

struct Window
{
	Vec(SDL_Event) events;
	Str title;
	SDL_Window* raw;
	union {
		SDL_Renderer* renderer;
		SDL_GLContext* context;
	};

	const Uint8* keystate;
	Uint32 mousestate;

	enum WindowFlags flags;
	int mousex, mousey;
	int width, height;
	int fps;
	int read; //Set to 1 after events has been handled
};

struct Window* window_ctor(
	struct Window* self,
	char* title, 
	int width, 
	int height, 
	enum WindowFlags flags
);
int window_update(struct Window* self);
void window_dtor(struct Window* self);

#endif

