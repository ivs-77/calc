#include <pthread.h>

class log
{
public:
	static bool all_sessions_complete();
	static void session_started();
	static void session_complete();
	static void log_percent();
	static void log_console(const char* message, ...);
	static void log_result(const char* user, int session_index, const char* expression, 
		const char* calc_answer, const char* status);
private:
	log();
	~log();
	static int get_total_test_count();
	static log instance;
	static pthread_mutex_t console_mutex;
	static pthread_mutex_t result_mutex;
	static pthread_mutex_t percent_mutex;
	static pthread_mutex_t sessions_mutex;
	static char result_file_name[20];
	static int total_test_count;
	static int current_test_count;
	static int total_sessions_running;
};