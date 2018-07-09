#include <errno.h>    
#include <string.h>    
#include <stdio.h>    
#include <stdarg.h>    
#include <time.h>    
#include "log.h"

log log::instance;
pthread_mutex_t log::console_mutex;

log::log()
{
	pthread_mutex_init(&console_mutex, NULL);
}

log::~log()
{
	pthread_mutex_destroy(&console_mutex);
}

void log::log_console(const char* message, ...)
{
	pthread_mutex_lock(&console_mutex);

	va_list arg;
    va_start(arg, message);
	vprintf(message, arg);
	va_end(arg);
	
	printf("\n");
	
    pthread_mutex_unlock(&console_mutex);
}
