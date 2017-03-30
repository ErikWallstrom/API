#ifndef STR_H
#define STR_H

#include "vec.h"
typedef Vec(char) Str;

#define str_getlen(s) (vec_getsize(s) - 1)

Str str_ctor(const char* s);
Str str_ctorwithfmt(const char* fmt, ...);
void str_set(Str* s, const char* s2);
void str_setwithfmt(Str* s, const char* fmt, ...);
void str_append(Str* s, const char* s2);
void str_prepend(Str* s, const char* s2);
void str_appendwithchar(Str* s, char c);
void str_prependwithchar(Str* s, char c);
int str_equal(Str* s, const char* s2);
void str_dtor(Str* s);

#endif
