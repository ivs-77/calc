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
	std::string _calc_user_hello;
	std::string _line_rem;
	double _total_session_calc_time;
	int _session_index;
	
	int login(int sockfd);
	int make_test(int sockfd);
	int read_line(int sockfd, std::string& line_str);
	int write_str(int sockfd, const char* str);
	int read_str(int sockfd, const char* str);
	int read_num(int sockfd, double& num);
	
public:
	
	static int start(int session_index, const char* user, const char* password);
	
};