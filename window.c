#include "window.h"
#include "error.h"
#include <assert.h>
#include <string.h> //Only for strlen on windows...

struct Window* window_ctor(
	char* title, 
	int width, 
	int height, 
	enum WindowFlags flags
)
{
	assert(title);
	assert(width > 0);
	assert(height > 0);

	struct Window* self = malloc(sizeof(struct Window));
	if(!self)
		debug("malloc", ERRORTYPE_MEMALLOC);

	self->raw = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, 
		height,
		(flags == WINDOW_CONTEXT) ? 
			SDL_WINDOW_OPENGL : 
			SDL_WINDOW_SHOWN
	);
	if(!self->raw)
		debug(SDL_GetError(), ERRORTYPE_CRITICAL);

	self->vsync = 0;
	if(flags & WINDOW_CONTEXT)
	{
		self->context = SDL_GL_CreateContext(self->raw);
		if(!self->context)
			debug(SDL_GetError(), ERRORTYPE_CRITICAL);
		if(flags & WINDOW_VSYNC)
		{
			SDL_GL_SetSwapInterval(1);
			self->vsync = 1;
		}
	}
	else if(flags & WINDOW_RENDERER)
	{
		Uint32 renderflags = SDL_RENDERER_ACCELERATED | 
			SDL_RENDERER_TARGETTEXTURE;
		if(flags & WINDOW_VSYNC)
		{
			self->vsync = 1;
			renderflags |= SDL_RENDERER_PRESENTVSYNC;
		}
		self->renderer = SDL_CreateRenderer(
			self->raw,
			-1,
			renderflags
		);
		if(!self->renderer)
			debug(SDL_GetError(), ERRORTYPE_CRITICAL);
	}
		
	self->fps = 0;
	self->frames = 0;
	self->oldticks = 0;
	self->flags = flags;
	self->width = width;
	self->height = height;
	self->keystate = SDL_GetKeyboardState(NULL);
	self->mousestate = SDL_GetMouseState(
		&self->mousex,
		&self->mousey
	);

	size_t title_len = strlen(title);
	self->title = vec_ctor(1, title_len + 1);
	vec_set(&self->title, title, title_len);
	self->events = vec_ctor(sizeof(SDL_Event), 0);
	return self;
}

int window_update(struct Window* self)
{
	assert(self);
	SDL_Event event;
	vec_set(&self->events, NULL, 0);
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			return 0;
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_TEXTEDITING:
		case SDL_TEXTINPUT:
			vec_pushback(&self->events, event);
		default:
			continue;
		}
	}

	self->mousestate = SDL_GetMouseState(
		&self->mousex,
		&self->mousey
	);

	SDL_GetWindowSize(self->raw, &self->width, &self->height);
	if(self->flags == WINDOW_CONTEXT)
		SDL_GL_SwapWindow(self->raw);
	else
	{
		SDL_RenderPresent(self->renderer);
		SDL_RenderClear(self->renderer);
	}

	if(SDL_GetTicks() / 1000 > self->oldticks)
	{
		self->oldticks = SDL_GetTicks() / 1000;
		self->fps = self->frames;
		self->frames = 0;
	}
	self->frames++;
	return 1;
}

void window_dtor(struct Window* self)
{
	assert(self);
	if(self->flags == WINDOW_CONTEXT)
		SDL_GL_DeleteContext(self->context);
	else
		SDL_DestroyRenderer(self->renderer);

	vec_dtor(&self->title);
	vec_dtor(&self->events);
	SDL_DestroyWindow(self->raw);
	free(self);
}

