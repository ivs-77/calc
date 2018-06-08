#include <string>
#include "account.h"

enum handler_state
{
	initial,
	handling,
	exit
};

class cmd_handler
{
private:
	
	cmd_handler(int connfd);
	~cmd_handler();
	void execute();
	static void* handler_proc(void* data);
	int _connfd;
	std::string current_command;
	
	int print(const char* message);
	int hello();
	int invalid_cmd();
	int wait();
	int read_command();
	handler_state login();
	handler_state handle_command();
	
	account* _account;

public:
	
	static void start(int connfd);
};