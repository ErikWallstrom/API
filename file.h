#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "str.h"

enum FileMode
{
	FILEMODE_READ = 1 << 0,
	FILEMODE_WRITE = 1 << 1,
};

struct File
{
	FILE* raw;
	Str content;
	enum FileMode mode;
};

struct File* file_ctor(
	struct File* self, 
	const char* filename, 
	enum FileMode mode
);
void file_dtor(struct File* self);

#endif
