#include "window.h"
#include "error.h"
#include "log.h"
#include <string.h> //Only for strlen on windows...

struct Window* window_ctor(
	char* title, 
	int width, 
	int height, 
	enum WindowFlags flags
)
{
	log_assert(title, "is NULL");
	log_assert(width > 0, "invalid");
	log_assert(height > 0, "invalid");

	struct Window* self = malloc(sizeof(struct Window));
	if(!self)
		log_error("malloc failed");

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
		log_error(SDL_GetError());

	self->vsync = 0;
	if(flags & WINDOW_CONTEXT)
	{
		self->context = SDL_GL_CreateContext(self->raw);
		if(!self->context)
			log_error(SDL_GetError());
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
			log_error(SDL_GetError());
	}
		
	self->fps = 0;
	self->read = 0;
	self->frames = 0;
	self->hidden = 0;
	self->oldticks = 0;
	self->flags = flags;
	self->width = width;
	self->height = height;
	self->keystate = SDL_GetKeyboardState(NULL);
	self->mousestate = SDL_GetMouseState(
		&self->mousex,
		&self->mousey
	);

	self->title = str_ctor(title);
	self->events = vec_ctor(SDL_Event, 0);
	return self;
}

int window_update(struct Window* self)
{
	log_assert(self, "is NULL");
	SDL_Event event;
	if(self->read)
	{
		//vec_set(&self->events, NULL, 0);
		vec_collapse(&self->events, 0, vec_getsize(&self->events));
		self->read = 0;
	}

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			return 0;
			break;
		case SDL_WINDOWEVENT:
			if(event.window.event == SDL_WINDOWEVENT_HIDDEN)
				self->hidden = 1;
			else if(event.window.event == SDL_WINDOWEVENT_HIDDEN)
				self->hidden = 0;
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

	/*
	if(self->hidden) //Temporary fix vsync minimize problem
		SDL_Delay(16);
	*/
	return 1;
}

void window_dtor(struct Window* self)
{
	log_assert(self, "is NULL");
	if(self->flags == WINDOW_CONTEXT)
		SDL_GL_DeleteContext(self->context);
	else
		SDL_DestroyRenderer(self->renderer);

	str_dtor(&self->title);
	vec_dtor(&self->events);
	SDL_DestroyWindow(self->raw);
	free(self);
}

