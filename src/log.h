#include <pthread.h>

class log
{
public:
	static void log_errno(const char* message);
	static void log_error(const char* message, ...);
	static void log_console(const char* message, ...);
private:
	log();
	~log();
	static char error_log_file_name[20];
	static log instance;
	static pthread_mutex_t error_mutex;
	static pthread_mutex_t console_mutex;
};