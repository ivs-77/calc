#include <errno.h>    
#include <string.h>    
#include <stdio.h>    
#include <stdarg.h>    
#include <time.h>    
#include "log.h"

char log::error_log_file_name[20];
char log::info_log_file_name[20];
log log::instance;
pthread_mutex_t log::error_mutex;
pthread_mutex_t log::info_mutex;

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
		
	sprintf(info_log_file_name, "inf_%d%02d%02d_%02d%02d%02d.txt",
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec);

	pthread_mutex_init(&error_mutex, NULL);
	pthread_mutex_init(&info_mutex, NULL);
}

log::~log()
{
	pthread_mutex_destroy(&error_mutex);
	pthread_mutex_destroy(&info_mutex);
}

void log::log_error(const char* message, ...)
{

	pthread_mutex_lock(&error_mutex);
	
	FILE* log_file = fopen(error_log_file_name, "a");
	if(log_file == NULL)
	{
		pthread_mutex_unlock(&error_mutex);
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

	pthread_mutex_unlock(&error_mutex);

}

void log::log_errno(const char* message)
{
	log_error("%s: %d %s", message, errno, strerror(errno));
}

void log::log_info(const char* message, ...)
{
	pthread_mutex_lock(&info_mutex);

	FILE* log_file = fopen(info_log_file_name, "a");
	if(log_file == NULL)
	{
		pthread_mutex_unlock(&info_mutex);
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

    pthread_mutex_unlock(&info_mutex);
}
