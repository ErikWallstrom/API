#include "file.h"
#include "log.h"
#include <errno.h>
#include <string.h>

struct File* file_ctor(
	struct File* self, 
	const char* filename, 
	enum FileMode mode
)
{
	log_assert(self, "is NULL");
	log_assert(filename, "is NULL");
	log_assert(
		mode & FILEMODE_READ || mode & FILEMODE_WRITE, 
		"invalid mode (%i)",
		mode
	);

	char* m;
	if(mode & FILEMODE_READ && mode & FILEMODE_WRITE)
	{
		m = "r+";
	}
	else if(mode & FILEMODE_READ)
	{
		m = "r";
	}
	else //mode & FILEMODE_WRITE
	{
		m = "wx";
	}

	self->raw = fopen(filename, m);
	if(!self->raw)
	{
		log_error(strerror(errno));
	}
	
	self->mode = mode;
	if(self->mode & FILEMODE_READ)
	{
		if(fseek(self->raw, 0, SEEK_END))
			log_error(strerror(errno));
		long filesize = ftell(self->raw);
		if(filesize == -1L)
			log_error(strerror(errno));
		if(fseek(self->raw, 0, SEEK_SET))
			log_error(strerror(errno));

		self->content = vec_ctor(char, filesize + 1);
		if(filesize)
		{
			vec_expand(&self->content, 0, filesize + 1);
			self->content[filesize] = '\0';
			size_t actuallyread = fread(
				self->content, 
				1, 
				filesize,
				self->raw
			);

			log_assert(
				actuallyread == (size_t)filesize, 
				"this should not happen (%zu, %li)",
				actuallyread,
				filesize
			);
		}
	}
	else
	{
		self->content = str_ctor("");
	}

	self->extension = str_ctor("");
	for(size_t i = 0; i < strlen(filename); i++)
	{
		if(filename[i] == '.')
		{
			for(size_t j = i; j < strlen(filename); j++)
			{
				str_appendwithchar(&self->extension, filename[j]);
			}

			break;
		}
	}

	return self;
}

void file_write(struct File* self)
{
	log_assert(self, "is NULL");
	log_assert(self->mode & FILEMODE_WRITE, "file not writeable");

	fseek(self->raw, 0, SEEK_SET);
	fputs(self->content, self->raw);
}

void file_dtor(struct File* self)
{
	log_assert(self, "is NULL");
	if(self->mode & FILEMODE_WRITE)
	{
		file_write(self);
	}

	str_dtor(&self->extension);
	str_dtor(&self->content);
	fclose(self->raw);
}
