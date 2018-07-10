#include <pthread.h>

class log
{
public:
	static void log_console(const char* message, ...);
	static void log_result(const char* user, int session_index, const char* expression, 
		const char* calc_answer, const char* exact_answer, const char* status);
private:
	log();
	~log();
	static log instance;
	static pthread_mutex_t console_mutex;
	static pthread_mutex_t result_mutex;
	static char result_file_name[20];
};