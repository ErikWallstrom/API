#include "argparser.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

void argparser_printhelp(
	char* progname, 
	struct ArgParserOption* options, 
	size_t numoptions
)
{
	log_assert(progname, "is NULL");
	log_assert(options, "is NULL");
	log_assert(numoptions, "is 0");

	printf("Usage: %s [OPTIONS -", progname);
	for(size_t i = 0; i < numoptions; i++)
	{
		putchar(options[i].shortopt);
	}

	puts("]\nOptions:");
	for(size_t i = 0; i < numoptions; i++)
	{
		printf("    ");
		if(options[i].shortopt != '\0')
		{
			printf("-%c, ", options[i].shortopt);
		}
		else
		{
			printf("    ");
		}

		if(options[i].longopt)
		{
			printf("--%s", options[i].longopt);
		}

		if(options[i].info)
		{
			int gap = (options[i].longopt) ? 
				15 - strlen(options[i].longopt) : 15;

			for(int j = 0; j < gap; j++)
			{
				putchar(' ');
			}

			puts(options[i].info);
		}
		else
		{
			putchar('\n');
		}
	}

	putchar('\n');
}

struct ArgParserResult argparser_parseoptions(
	int argc, 
	char* argv[],
	struct ArgParserOption* options,
	size_t numoptions
)
{
	log_assert(argc > 0, "invalid");
	log_assert(argv, "is NULL");
	log_assert(options, "is NULL");
	log_assert(numoptions, "is 0");

	struct ArgParserResult result;
	result.invalidoptions = 0;
	result.args = vec_ctor(char*, 0);

	int nextisarg = 0;
	for(int i = 1; i < argc; i++)
	{
		if(nextisarg || argv[i][0] != '-')	
		{
			vec_pushback(&result.args, argv[i]);
			nextisarg = 0;
			continue;
		}

		if(argv[i][1] == '-')
		{
			if(argv[i][2] == '\0')
			{
				nextisarg = 1;
			}
			else
			{
				int found = 0;
				for(size_t j = 0; j < numoptions; j++)
				{
					if(!options[j].longopt)
						continue;

					if(!strcmp(&argv[i][2], options[j].longopt))
					{
						options[j].used = 1;
						found = 1;
						break;
					}
				}

				if(!found)
				{
					result.invalidoptions++;
				}
			}
			continue;
		}

		int found = 0;
		for(size_t j = 0; j < numoptions; j++)
		{
			if(argv[i][1] == '\0')
				continue;

			if(argv[i][1] == options[j].shortopt && argv[i][2] == '\0')
			{
				options[j].used = 1;
				found = 1;
				break;
			}
		}

		if(!found)
		{
			result.invalidoptions++;
		}
	}

	return result;
}

