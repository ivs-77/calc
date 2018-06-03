#include <errno.h>    
#include <string.h>    
#include <stdio.h>    
#include <stdarg.h>    
#include <time.h>    
#include "log.h"

char log::error_log_file_name[20];
log log::instance;

log::log()
{
	time_t rawtime;  
	struct tm* timeinfo;  
	time (&rawtime);  
	timeinfo = localtime(&rawtime);  
	sprintf(error_log_file_name, "err_%d%02d%02d_%02d%02d%02d.txt",
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec);
}

void log::log_error(const char* message, ...)
{

	FILE* log_file = fopen(error_log_file_name, "a");
	if(log_file == NULL)
	{
		log_console("Cannot open log file: %d %s", errno, strerror(errno));
		return;
	}

	time_t rawtime;  
	struct tm* timeinfo;  
	time (&rawtime);  
	timeinfo = localtime(&rawtime);  
	
	fprintf(log_file, "%d.%02d.%02d %02d:%02d:%02d ", 
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec);
	
	va_list arg;
    va_start(arg, message);
	vfprintf(log_file, message, arg);
	va_end(arg);
	
	fprintf(log_file, "\n");
	
	fclose(log_file);
}

void log::log_errno(const char* message)
{
	log_error("%s: %d %s", message, errno, strerror(errno));
}

void log::log_console(const char* message, ...)
{
	va_list arg;
    va_start(arg, message);
    vprintf(message, arg);
    va_end(arg);
    printf("\n");
}
