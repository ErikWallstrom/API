#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

enum ErrorType
{
	ERRORTYPE_ARGUMENT,
	ERRORTYPE_MEMALLOC,
	ERRORTYPE_CRITICAL,
	ERRORTYPE_APPLICATION
};

extern char error_buffer[128];
#define debug(msg, type) \
	do { \
		snprintf( \
			error_buffer, \
			sizeof(error_buffer), \
			"<%s>(%i): %s\n", \
			__func__, \
			__LINE__, \
			msg \
		); \
		error(error_buffer, type); \
	} while(0)

void error(const char* msg, enum ErrorType type);

#endif
