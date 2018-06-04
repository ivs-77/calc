
class config
{
private:

	static int _port;
	static int _loginTimeout;

public:

	static int read();
	static int get_port();
	static int get_login_timeout();
};	
