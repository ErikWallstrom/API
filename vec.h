#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#define Vec(T) T *

#define vec_insert(vec, pos, ...) \
	vec_expand(vec, pos, 1), (*(vec))[pos] = __VA_ARGS__
#define vec_push(vec, ...) vec_insert(vec, 0, __VA_ARGS__)
#define vec_pushback(vec, ...) \
	vec_expand( \
		vec, vec_getsize(vec), 1 \
	), \
	(*(vec))[vec_getsize(vec) - 1] = __VA_ARGS__
#define vec_pop(vec) vec_collapse(vec, 0, 1)
#define vec_popback(vec) vec_collapse(vec, vec_getsize(vec) - 1, 1)


Vec(void) vec_ctor(size_t element_size, size_t elements);
Vec(void) vec_copy(Vec(void) vec);
void vec_dtor(Vec(void) vec);
size_t vec_getsize(Vec(void) vec);

void vec_expand(Vec(void) vec, size_t pos, size_t elements);
void vec_collapse(Vec(void) vec, size_t pos, size_t elements);

void vec_set(Vec(void) vec, void* data, size_t elements);
void vec_append(Vec(void) vec, void* data, size_t elements);
void vec_prepend(Vec(void) vec, void* data, size_t elements);

#endif

