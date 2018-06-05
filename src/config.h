
class config
{
private:

	static int _port;
	static int _loginTimeout;
	static int _max_cmd_length;

public:

	static int read();
	static int get_port();
	static int get_login_timeout();
	static int get_max_cmd_length();
};	
