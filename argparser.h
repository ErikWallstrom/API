#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <stddef.h>
#include "vec.h"

struct ArgParserOption
{
	char shortopt;
	char* longopt;
	char* info;
	int used;
};

struct ArgParserResult
{
	int invalidoptions;
	Vec(char*) args;
};

void argparser_printhelp(
	char* progname, 
	struct ArgParserOption* options, 
	size_t numoptions
);
struct ArgParserResult argparser_parseoptions(
	int argc, 
	char* argv[],
	struct ArgParserOption* options,
	size_t numoptions
);

#endif
