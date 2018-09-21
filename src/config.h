#include <string>

class config
{
private:

	static int _port;
	static int _login_timeout;
	static std::string _connect;
	static std::string _stop_key;
	static std::string _result_format;

public:

	static int read();
	static int get_port();
	static int get_login_timeout();
	static const char* get_connect();
	static const char* get_stop_key();
	static const char* get_result_format();
};	
