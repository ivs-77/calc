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

class cmd_handler
{
public:
	
	cmd_handler();
	static void accept(ip::tcp::acceptor& acceptor);

private:
	
	void execute();
	static void handler_proc(std::unique_ptr<cmd_handler>&& handler);
	void inst_accept(ip::tcp::acceptor& acceptor);
	
	int print(const char* message);
	int hello();
	int read_command();
	int read_command_limited_in_time();
	handler_state calc(std::string calc_expression);
	handler_state login();
	handler_state handle_command();
	
	account* _account = NULL;
	io_service _service;
	ip::tcp::socket _socket;
	streambuf _buffer;
	std::istream _buffer_stream;
	std::string current_command;

};