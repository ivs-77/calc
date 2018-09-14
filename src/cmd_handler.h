#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "account.h"

enum handler_state
{
	initial,
	handling,
	exit_state
};

using namespace boost::asio;

typedef std::unique_ptr<ip::tcp::socket> socket_ptr;

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
	int wait();
	int read_command();
	handler_state calc(std::string calc_expression);
	handler_state login();
	handler_state handle_command();
	
	account* _account;

public:
	
	static void start(int connfd);
	
	static void start_v2(socket_ptr&& socket);
};