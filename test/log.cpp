#include <errno.h>    
#include <string.h>    
#include <stdio.h>    
#include <stdarg.h>    
#include <time.h>    
#include "log.h"

log log::instance;
pthread_mutex_t log::console_mutex;
pthread_mutex_t log::result_mutex;
char log::result_file_name[20];

log::log()
{
	pthread_mutex_init(&console_mutex, NULL);
	pthread_mutex_init(&result_mutex, NULL);

	time_t rawtime;  
	struct tm* timeinfo;  
	time (&rawtime);  
	timeinfo = localtime(&rawtime);  
	sprintf(result_file_name, "res_%d%02d%02d_%02d%02d%02d.txt",
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec);
}

log::~log()
{
	pthread_mutex_destroy(&console_mutex);
	pthread_mutex_destroy(&result_mutex);
}

void log::log_result(const char* user, int session_index, const char* expression, 
	const char* calc_answer, const char* exact_answer, const char* status)
{
	pthread_mutex_lock(&result_mutex);
	
	FILE* res_file = fopen(result_file_name, "a");
	if(res_file == NULL)
	{
		pthread_mutex_unlock(&result_mutex);
		log_console("Result file can't be opened");
		return;
	}

	fprintf(res_file, "%s[%d]\t%s\t%s\t%s\t%s\n", 
		user, session_index, expression, calc_answer, exact_answer, status);

	fclose(res_file);
	
	pthread_mutex_unlock(&result_mutex);
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
