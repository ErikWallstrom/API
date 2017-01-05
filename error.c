#include "error.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

char error_buffer[128];

void error(const char* msg, enum ErrorType type)
{
	switch(type)
	{
	case ERRORTYPE_MEMALLOC:
		printf("MEMALLOC ERROR: (%s)\n", msg);
		abort();
		break;
	case ERRORTYPE_CRITICAL:
		printf("CRITICAL ERROR: (%s)\n", msg);
		abort();
		break;
	case ERRORTYPE_ARGUMENT:
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Argument Error",
			msg,
			NULL
		);
		abort();
		break;
	case ERRORTYPE_APPLICATION:
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Application Error",
			msg,
			NULL
		);
		abort();
		break;
	}
}
