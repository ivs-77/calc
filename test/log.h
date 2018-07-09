#include <pthread.h>

class log
{
public:
	static void log_console(const char* message, ...);
private:
	log();
	~log();
	static log instance;
	static pthread_mutex_t console_mutex;
};