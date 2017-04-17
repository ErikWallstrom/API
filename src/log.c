#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void log_assert_(
	const char* statement, 
	int result,
	int line, 
	const char* func, 
	const char* fmt, 
	...
)
{
	FILE* log_dest = stdout; //Could be changed
	if(!result)
	{
		fprintf(
			log_dest, 
			LOGMSGCOLOR_YELLOW LOGMSGCOLOR_DARK "*DEBUG*"
				LOGMSGCOLOR_RESET
				" Assertion failed (%s\"%s\"%s: ",
			LOGMSGCOLOR_RED LOGMSGCOLOR_BRIGHT,
			statement,
			LOGMSGCOLOR_RESET
		);

		va_list vlist;
		va_start(vlist, fmt);
		fputs(LOGMSGCOLOR_YELLOW LOGMSGCOLOR_BRIGHT, log_dest);
		vfprintf(
			log_dest,
			fmt,
			vlist
		);
		va_end(vlist);
		fputs(LOGMSGCOLOR_RESET, log_dest);

		fprintf(
			log_dest,
			") in <" LOGMSGCOLOR_CYAN LOGMSGCOLOR_BRIGHT "%s"
				LOGMSGCOLOR_RESET
				"> at [" LOGMSGCOLOR_BLUE LOGMSGCOLOR_BRIGHT
				"%i" LOGMSGCOLOR_RESET "]\n",
			func,
			line
		);

		abort();
	}
}

void log_msg(enum LogMsgType type, const char* fmt, ...)
{
	log_assert(fmt, "fmt is NULL");
	FILE* log_dest = stdout;
	const char* output = NULL;
	switch(type)
	{
	case LOGMSGTYPE_INFO:
		output = LOGMSGCOLOR_GREEN LOGMSGCOLOR_BRIGHT
			"*INFO*    " LOGMSGCOLOR_RESET " ";
		break;
	case LOGMSGTYPE_WARNING:
		output = LOGMSGCOLOR_YELLOW LOGMSGCOLOR_BRIGHT
			"*WARNING* " LOGMSGCOLOR_RESET " ";
		break;
	case LOGMSGTYPE_ERROR:
		output = LOGMSGCOLOR_RED LOGMSGCOLOR_BRIGHT
			"*ERROR*   " LOGMSGCOLOR_RESET " ";
		break;
	default:
		log_assert(0, "invalid type (%i)", type);
	}

	fputs(output, log_dest);
	va_list vlist;
	va_start(vlist, fmt);

	vfprintf(
		log_dest,
		fmt,
		vlist
	);
	va_end(vlist);

	fputs("\n", log_dest);
	if(type == LOGMSGTYPE_ERROR)
	{
		abort();
	}
}

