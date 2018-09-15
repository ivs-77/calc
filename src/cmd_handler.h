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
	
	cmd_handler(socket_ptr&& socket);
	~cmd_handler();
	void execute();
	static void handler_proc(socket_ptr&& socket);
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
	
	socket_ptr _socket;

public:
	
	static void start(socket_ptr&& socket);
};