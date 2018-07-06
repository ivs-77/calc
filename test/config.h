#include <string>
#include <vector>

class calc_user
{
public:
	calc_user(const char* name, const char* password);
	const char* get_name();
	const char* get_password();
private:
	std::string  _name;
	std::string  _password;
};

class config
{
private:

	static std::string _host;
	static int _port;
	static int _sessions_per_user;
	static int _tests_per_session;
	static std::vector<calc_user> _calc_users;

public:

	static int read();
	static const char* get_host();
	static int get_port();
	static int get_sessions_per_user();
	static int get_tests_per_session();
	static unsigned int get_users_count();
	static const char* get_user_name(unsigned int user_index);
	static const char* get_user_password(unsigned int user_index);

};	
