#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define LOGMSGCOLOR_BLACK "\x1b[30"
#define LOGMSGCOLOR_RED "\x1b[31"
#define LOGMSGCOLOR_GREEN "\x1b[32"
#define LOGMSGCOLOR_YELLOW "\x1b[33"
#define LOGMSGCOLOR_BLUE "\x1b[34"
#define LOGMSGCOLOR_MAGENTA "\x1b[35"
#define LOGMSGCOLOR_CYAN "\x1b[36"
#define LOGMSGCOLOR_WHITE "\x1b[37"
#define LOGMSGCOLOR_DARK "m"
#define LOGMSGCOLOR_BRIGHT ";1m"
#define LOGMSGCOLOR_RESET "\x1b[0m"

#define log_info(fmt, ...) log_msg(LOGMSGTYPE_INFO, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...) log_msg(LOGMSGTYPE_warning, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) log_msg(LOGMSGTYPE_ERROR, fmt, ##__VA_ARGS__)
#define log_assert(statement, fmt, ...) \
	log_assert_( \
		#statement, \
		statement ? 1 : 0, \
		__LINE__, \
		__func__, \
		fmt, \
		##__VA_ARGS__ \
	)

enum LogMsgType
{
	LOGMSGTYPE_INFO,
	LOGMSGTYPE_WARNING,
	LOGMSGTYPE_ERROR,
};

void log_init(FILE* dest);
void log_assert_(
	const char* statement, 
	int result,
	int line, 
	const char* func, 
	const char* fmt, 
	...
);
void log_msg(enum LogMsgType type, const char* fmt, ...);

#endif
