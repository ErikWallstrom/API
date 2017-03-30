#include "../vec.h"
#include "../str.h"
#include <stdio.h>
#include <string.h>

void print(char* str)
{
	puts(str);
}

int main(void)
{
	Str str = str_ctor("I__I");
	str_append(str, str);

	/*
	for(size_t i = 0; i < vec_getsize(str); i++)
		printf("%i, %c\n", str[i], str[i]);
	*/

	puts(str);
	str_dtor(str);
}

