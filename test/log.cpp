#include <errno.h>    
#include <string.h>    
#include <stdio.h>    
#include <stdarg.h>    
#include <sys/time.h>
#include <time.h>    
#include "log.h"
#include "config.h"

log log::instance;
pthread_mutex_t log::console_mutex;
pthread_mutex_t log::result_mutex;
pthread_mutex_t log::percent_mutex;
pthread_mutex_t log::sessions_mutex;
char log::result_file_name[20];
int log::total_test_count = -1;
int log::current_test_count = 0;
int log::total_sessions_running = 0;
double log::total_sessions_calc_time = 0;
int log::failed_tests_count = 0;

log::log()
{
	pthread_mutex_init(&console_mutex, NULL);
	pthread_mutex_init(&result_mutex, NULL);
	pthread_mutex_init(&percent_mutex, NULL);
	pthread_mutex_init(&sessions_mutex, NULL);

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
	pthread_mutex_destroy(&percent_mutex);
	pthread_mutex_destroy(&sessions_mutex);
}

void log::log_result(const char* user, int session_index, const char* expression, 
	const char* calc_answer, const char* status)
{
	pthread_mutex_lock(&result_mutex);
	
	FILE* res_file = fopen(result_file_name, "a");
	if(res_file == NULL)
	{
		pthread_mutex_unlock(&result_mutex);
		log_console("Result file can't be opened");
		return;
	}

	fprintf(res_file, "%s[%d]\n\tExpr:\t%s\n\tAnswer:\t%s\n\tStatus:\t%s\n", 
		user, session_index, expression, calc_answer, status);

	fclose(res_file);
	
	pthread_mutex_lock(&percent_mutex);
	current_test_count++;
	if(strcmp(status, "OK") != 0)
		++failed_tests_count;
	pthread_mutex_unlock(&percent_mutex);
	
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
	fflush(stdout);
	
    pthread_mutex_unlock(&console_mutex);
}

void log::log_percent()
{
	pthread_mutex_lock(&percent_mutex);
    double percent = 100.0*current_test_count/get_total_test_count();
    pthread_mutex_unlock(&percent_mutex);
    
    pthread_mutex_lock(&console_mutex);
    printf("\rTesting: %6.2f%%", percent);
	fflush(stdout);
    pthread_mutex_unlock(&console_mutex);
}
	
int log::get_total_test_count()
{
	if(total_test_count == -1)
	{
		total_test_count = 
			config::get_sessions_per_user() * 
			config::get_tests_per_session() * 
			config::get_users_count();
	}
	return total_test_count;
}

bool log::all_sessions_complete()
{
	pthread_mutex_lock(&sessions_mutex);
    bool result = (total_sessions_running == 0);
    pthread_mutex_unlock(&sessions_mutex);
    return result;
}

void log::session_started()
{
	pthread_mutex_lock(&sessions_mutex);
    total_sessions_running++;
    pthread_mutex_unlock(&sessions_mutex);
}

void log::session_complete(double total_calc_time)
{
	pthread_mutex_lock(&sessions_mutex);
    total_sessions_running--;
    total_sessions_calc_time += total_calc_time;
    pthread_mutex_unlock(&sessions_mutex);
}

void log::log_totals(const timeval& start_time)
{
	
	timeval end_time;
	gettimeofday(&end_time, NULL);
	
	double time_diff = 
		(1.0 * end_time.tv_sec + end_time.tv_usec / 1e6) - 
		(1.0 * start_time.tv_sec + start_time.tv_usec / 1e6);

	pthread_mutex_lock(&result_mutex);
	
	FILE* res_file = fopen(result_file_name, "a");
	if(res_file == NULL)
	{
		pthread_mutex_unlock(&result_mutex);
		log_console("Result file can't be opened");
		return;
	}

    fprintf(res_file, "-----------------------------------\n");
    fprintf(res_file, "Total execution time  : %10.3f sec\n", time_diff);
    fprintf(res_file, "Total calc time       : %10.3f sec\n", total_sessions_calc_time);
    fprintf(res_file, "Total tests count     : %10d\n", get_total_test_count());
    fprintf(res_file, "Failed tests count    : %10d\n", failed_tests_count);
    fprintf(res_file, "Total sessions count  : %10d\n", config::get_users_count() * config::get_sessions_per_user());
    fprintf(res_file, "Avg calc time per test: %10.3f sec\n", total_sessions_calc_time / get_total_test_count());

	fclose(res_file);
	pthread_mutex_unlock(&result_mutex);

	pthread_mutex_lock(&console_mutex);
    printf("\rTotal execution time  : %10.3f sec\n", time_diff);
    printf("Total calc time       : %10.3f sec\n", total_sessions_calc_time);
    printf("Total tests count     : %10d\n", get_total_test_count());
    printf("Failed tests count    : %10d\n", failed_tests_count);
    printf("Total sessions count  : %10d\n", config::get_users_count() * config::get_sessions_per_user());
    printf("Avg calc time per test: %10.3f sec\n", total_sessions_calc_time / get_total_test_count());
    pthread_mutex_unlock(&console_mutex);
}
