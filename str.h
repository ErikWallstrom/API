#ifndef STR_H
#define STR_H

#include "vec.h"
typedef Vec(char) Str;

Str str_ctor(const char* s);
Str str_ctorwithfmt(const char* fmt, ...);
void str_set(Str* s, const char* s2);
void str_setwithfmt(Str* s, const char* fmt, ...);
void str_append(Str* s, const char* s2);
void str_prepend(Str* s, const char* s2);
void str_dtor(Str* s);

#endif
