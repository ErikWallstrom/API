#include "vec.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

#define VEC_DEFAULT_SIZE 5
#define VEC_NEW_SIZE 5
#define VEC_NULL_ARGUMENT "vec is NULL"

#define nullcheck(p, msg) \
	do { \
		if(!p) \
			debug(msg, ERRORTYPE_ARGUMENT); \
	} while(0)

#define magiccheck(vec) \
	do { \
		if((vec)->magic == MAGIC_ALLOCATED) \
			if(*(unsigned int*)( \
					(vec)->buffer + (vec)->buffer_size) == MAGIC_ALLOCATED \
			) \
				break; \
			else \
				if(*(unsigned int*)( \
					(vec)->buffer + (vec)->buffer_size) == MAGIC_FREED \
				) \
					debug( \
						"Vector passed has had an unknown error", \
						ERRORTYPE_ARGUMENT \
					); \
				else \
					debug( \
						"Vector passed has been written to out of bound\n", \
						ERRORTYPE_CRITICAL \
					); \
		else if((vec)->magic == MAGIC_FREED) \
			debug("Vector passed is already destroyed", ERRORTYPE_ARGUMENT); \
		else \
			debug("Value passed is not a vector", ERRORTYPE_ARGUMENT); \
	} while(0)

#define tovector(vp) \
	(struct _Vector*)((*(char**)(vp)) - offsetof(struct _Vector, buffer))

enum MagicNumbers
{
	MAGIC_ALLOCATED = 0xA110CA7E,
	MAGIC_FREED 	= 0xDEADEDED
};

struct _Vector
{
	size_t size;
	size_t buffer_size;
	size_t element_size;
	unsigned int magic;
	char buffer[];
};

Vec(void) vec_ctor(size_t element_size, size_t elements)
{
	if(element_size < 1)
	{
		printf(
			"Error: element_size must be at least 1, at {%s} [%s, %i]\n", 
			__func__, 
			__FILE__, 
			__LINE__
		);
		abort();
	}

	size_t buffer_size = (elements ?
			elements : VEC_DEFAULT_SIZE) * element_size;
	struct _Vector* self = malloc(sizeof(struct _Vector) + 
			buffer_size + sizeof(unsigned int));
	nullcheck(self, "malloc");

	self->buffer_size = buffer_size;
	self->element_size = element_size;
	self->size = 0;
	self->magic = MAGIC_ALLOCATED;
	*(unsigned int*)(self->buffer + self->buffer_size) = MAGIC_ALLOCATED;

	return self->buffer;
}

Vec(void) vec_copy(Vec(void) vec)
{
	nullcheck(vec, VEC_NULL_ARGUMENT);
	struct _Vector* self = tovector(vec);
	magiccheck(self);

	void* dest = vec_ctor(
		self->element_size, 
		self->buffer_size / self->element_size
	);
	struct _Vector* d = tovector(&dest);
	d->size = self->size;
	memcpy(dest, self->buffer, self->buffer_size);
	*(unsigned int*)(self->buffer + self->buffer_size) = MAGIC_ALLOCATED;
	return dest;
}

void vec_dtor(Vec(void) vec)
{
	nullcheck(vec, VEC_NULL_ARGUMENT);
	struct _Vector* self = tovector(vec);
	magiccheck(self);

	self->magic = MAGIC_FREED;
	*(unsigned int*)(self->buffer + self->buffer_size) = MAGIC_FREED;
	free(self);
}

size_t vec_getsize(Vec(void) vec)
{
	nullcheck(vec, VEC_NULL_ARGUMENT);
	struct _Vector* self = tovector(vec);
	magiccheck(self);

	return self->size;
}

void vec_expand(Vec(void) vec, size_t pos, size_t elements)
{
	nullcheck(vec, VEC_NULL_ARGUMENT);
	if(elements < 1)
	{
		printf(
			"Error: Expanding with 0 is useless, at {%s} [%s, %i]\n", 
			__func__, 
			__FILE__, 
			__LINE__
		);
		abort();
	}
	struct _Vector* self = tovector(vec);
	magiccheck(self);

	if(self->size < pos)
	{
		printf(
			"Error: Index is out of bounds, at {%s} [%s, %i]\n", 
			__func__, 
			__FILE__,
			__LINE__
		);
		abort();
	}

	self->size += elements;
	if(self->buffer_size < self->size * self->element_size)
	{
		self->buffer_size = self->size * self->element_size + 
			self->element_size * VEC_NEW_SIZE;
		self = realloc(
			self, 
			sizeof(struct _Vector) + self->buffer_size + sizeof(unsigned int)
		);
		*(void**)vec = self->buffer;
		nullcheck(self, "realloc");
	}

	memmove(
		self->buffer + (pos + elements) * self->element_size, 
		self->buffer + pos * self->element_size, 
		(self->size - elements - pos) * self->element_size
	);
	*(unsigned int*)(self->buffer + self->buffer_size) = MAGIC_ALLOCATED;
}

void vec_collapse(Vec(void) vec, size_t pos, size_t elements)
{
	nullcheck(vec, VEC_NULL_ARGUMENT);
	if(elements < 1)
	{
		printf(
			"Error: Collapsing 0 is useless, at {%s} [%s, %i]\n", 
			__func__, 
			__FILE__, 
			__LINE__
		);
		abort();
	}
	struct _Vector* self = tovector(vec);
	if(self->size <= pos)
	{
		printf(
			"Error: Index is out of bounds, at {%s} [%s, %i]\n", 
			__func__, 
			__FILE__, 
			__LINE__
		);
		abort();
	}
	magiccheck(self);

	memmove(
		self->buffer + pos * self->element_size, 
		self->buffer + (pos + elements) * self->element_size,
		(self->size - elements - pos) * self->element_size
	);
	self->size -= elements;
}

void vec_set(Vec(void) vec, void* data, size_t elements)
{
	nullcheck(vec, VEC_NULL_ARGUMENT);
	struct _Vector* self = tovector(vec);
	magiccheck(self);

	self->size = elements;
	if(self->buffer_size < self->element_size * elements)
	{
		self->buffer_size = elements * self->element_size;
		self = realloc(
			self, 
			sizeof(struct _Vector) + self->buffer_size + sizeof(unsigned int)
		);
		*(void**)vec = self->buffer;
		nullcheck(self, "realloc");
	}

	memmove(self->buffer, data, self->size * self->element_size);
	*(unsigned int*)(self->buffer + self->buffer_size) = MAGIC_ALLOCATED;
}

void vec_append(Vec(void) vec, void* data, size_t elements)
{
	nullcheck(vec, VEC_NULL_ARGUMENT);
	nullcheck(data, VEC_NULL_ARGUMENT);
	if(elements < 1)
	{
		printf(
			"Error: Setting an empty array is useless, at {%s} [%s, %i]\n", 
			__func__, 
			__FILE__, 
			__LINE__
		);
		abort();
	}
	struct _Vector* self = tovector(vec);
	magiccheck(self);

	vec_expand(vec, self->size, elements);
	self = tovector(vec);
	memcpy(
		self->buffer + (self->size - elements) * self->element_size, 
		data, 
		elements * self->element_size
	);
}

void vec_prepend(Vec(void) vec, void* data, size_t elements)
{
	nullcheck(vec, VEC_NULL_ARGUMENT);
	nullcheck(data, VEC_NULL_ARGUMENT);
	if(elements < 1)
	{
		printf(
			"Error: Setting an empty array is useless, at {%s} [%s, %i]\n", 
			__func__, 
			__FILE__, 
			__LINE__
		);
		abort();
	}
	struct _Vector* self = tovector(vec);
	magiccheck(self);

	vec_expand(vec, 0, elements);
	self = tovector(vec);
	memcpy(self->buffer, data, elements * self->element_size);
}

