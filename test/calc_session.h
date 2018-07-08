#include <string>

class calc_session
{
private:
	
	calc_session(int session_index, const char* user, const char* password);
	~calc_session();
	void execute();
	static void* session_proc(void* data);
	std::string _user;
	std::string _password;
	int _session_index;

public:
	
	static int start(int session_index, const char* user, const char* password, pthread_t* session_thread);
	
};