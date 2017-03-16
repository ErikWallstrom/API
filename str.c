#include "str.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

Str str_ctor(const char* s)
{
	log_assert(s, "s is NULL");
	Str self = vec_ctor(char, strlen(s) + 1);
	vec_expand(&self, 0, strlen(s) + 1);
	strcpy(self, s);
	return self;
}

Str str_ctorwithfmt(const char* fmt, ...)
{
	log_assert(fmt, "s is NULL");
	va_list vlist, vlist2;
	va_start(vlist, fmt);
	va_copy(vlist2, vlist);

	int len = vsnprintf(NULL, 0, fmt, vlist) + 1;

	Str self = vec_ctor(char, len);
	vec_expand(&self, 0, len);
	vsprintf(self, fmt, vlist2);

	va_end(vlist2);
	va_end(vlist);
	return self;
}

void str_set(Str* s, const char* s2)
{
	log_assert(s, "s is NULL");
	log_assert(s2, "s is NULL");

	vec_collapse(s, 0, vec_getsize(s));
	vec_expand(s, 0, strlen(s2) + 1);
	strcpy(*s, s2);
}

void str_setwithfmt(Str* s, const char* fmt, ...)
{
	log_assert(s, "s is NULL");
	log_assert(fmt, "s is NULL");

	va_list vlist, vlist2;
	va_start(vlist, fmt);
	va_copy(vlist2, vlist);

	vec_collapse(s, 0, vec_getsize(s));
	int len = vsnprintf(NULL, 0, fmt, vlist) + 1;
	vec_expand(s, 0, len);
	vsprintf(*s, fmt, vlist2);

	va_end(vlist2);
	va_end(vlist);
}

void str_append(Str* s, const char* s2)
{
	log_assert(s, "s is NULL");
	log_assert(s2, "s is NULL");

	size_t oldsize = str_getlen(s);
	vec_expand(s, oldsize, strlen(s2));
	strcpy(*s + oldsize, s2);
}

void str_prepend(Str* s, const char* s2)
{
	log_assert(s, "s is NULL");
	log_assert(s2, "s is NULL");

	vec_expand(s, 0, strlen(s2));
	memcpy(*s, s2 ,strlen(s2));
}

void str_appendwithchar(Str* s, char c)
{
	log_assert(s, "s is NULL");
	vec_insert(s, str_getlen(s) - 1, c);
}

void str_prependwithchar(Str* s, char c)
{
	log_assert(s, "s is NULL");
	vec_push(s, c);
}

void str_dtor(Str* s)
{
	log_assert(s, "is NULL");
	vec_dtor(s);
}

