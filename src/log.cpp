#include <errno.h>    
#include <string.h>    
#include <stdio.h>    
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

void log::log_error(const char* message)
{
	FILE* log_file = fopen(error_log_file_name, "a");
	if(log_file == NULL)
	{
		printf("Cannot open log file: %s\n", strerror(errno));
		return;
	}

	time_t rawtime;  
	struct tm* timeinfo;  
	time (&rawtime);  
	timeinfo = localtime(&rawtime);  
	
	if(errno == 0)
	{
		fprintf(log_file, "%d.%02d.%02d %02d:%02d:%02d %s\n", 
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec,
			message);
	}
	else
	{
		fprintf(log_file, "%d.%02d.%02d %02d:%02d:%02d %s: %s\n", 
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec,
			message,
			strerror(errno));
	};

	fclose(log_file);
}
