#include <sys/types.h>    
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <thread>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include "log.h"
#include "cmd_handler.h"
#include "config.h"
#include "calc_node.h"

cmd_handler::cmd_handler()
	: _socket(_service), _buffer_stream(&_buffer)
{
}

void cmd_handler::execute()
{

	_connfd = _socket.native_handle();
	handler_state state = initial;
	
	while(state != exit_state)
	{
		switch(state)
		{
			case initial:
				state = login();
				break;
			case handling:
				state = handle_command();
				break;	
			case exit_state:
				break;
		}
	}
}

void trim_last_cr(std::string& trimming_str)
{
	// for netcat -C mode: it ends line with \r\n
	if(trimming_str.length() > 0 && trimming_str[trimming_str.length() - 1] == '\r')
		trimming_str.resize(trimming_str.length() - 1);
}

int cmd_handler::read_command_limited_in_time()
{

	int result = -1;
	deadline_timer timer(_service);
	timer.expires_from_now(boost::posix_time::seconds(config::get_login_timeout()));
	timer.async_wait([this] (const boost::system::error_code& error)
		{ 
			if(!error)
				_socket.cancel();
			else if(error != error::operation_aborted)
				log::log_error("Read command timer error: %d %s", error.value(), error.message().c_str());
		});
		
	async_read_until(_socket, _buffer, '\n', 
		[this, &result, &timer] (const boost::system::error_code& error, size_t)
		{
			if(error != error::operation_aborted)
				timer.cancel();
			if(error == error::eof)
			{
				// client closed connection
				return;
			}
			if(error)
			{
				log::log_error("Read command error: %d %s", error.value(), error.message().c_str());
				return;
			}
			std::getline(_buffer_stream, current_command);
			trim_last_cr(current_command);
			result = 0;
		});
		
	_service.reset();
	_service.run();
	return result;
}

int cmd_handler::read_command()
{
	boost::system::error_code error;
	read_until(_socket, _buffer, '\n', error);
	if(error == error::eof)
	{
		// client closed connection
		return -1;
	}
	else if(error)
	{
		log::log_error("Error reading user input: %d %s", error.value(), error.message().c_str());
		return -1;
	}
	std::getline(_buffer_stream, current_command);
	trim_last_cr(current_command);
	return 0;
}

int cmd_handler::print(const char* message)
{
	if(write(_connfd, message, strlen(message)) == -1)
	{
		log::log_errno("Error writing to socket");
		return -1;
	}
	return 0;
}

int cmd_handler::hello()
{
	if(_account == NULL)
	{
		return print("calc>");
	}
	else
	{
		int res = print(_account->get_name());
		return res != 0 ? res : print("@calc>");
	}
}

handler_state cmd_handler::login()
{
	
	if(hello() == -1 || read_command_limited_in_time() == -1)
		return exit_state;	
		
	if(current_command == config::get_stop_key())
	{
		log::log_info("Stop_key command accepted. Calc is terminating.");
		exit(0);
	}

	const char* login_cmd = "login ";
	const size_t login_length = strlen(login_cmd);
	if(current_command.compare(0, login_length, login_cmd) != 0 || 
	   current_command.length() <= login_length)
	{
		if(print("Invalid command\n") == -1)
			return exit_state;
		return initial;
	}
	
	std::string login_name = current_command.substr(login_length);
	
	if(hello() == -1 || read_command_limited_in_time() == -1)
		return exit_state;	
	
	const char* pwd_cmd = "password ";
	const size_t pwd_length = strlen(pwd_cmd);
	if(current_command.compare(0, pwd_length, pwd_cmd) != 0 || 
	   current_command.length() <= pwd_length)
	{
		if(print("Invalid command\n") == -1)
			return exit_state;
		return initial;
	}
	
	std::string pwd = current_command.substr(pwd_length);

	_account = account::get_account(login_name, pwd);
	if(_account == NULL)
	{
		print("Access denied\n");
		return initial;
	}

	return handling;
}

handler_state cmd_handler::handle_command()
{
	
	if(hello() == -1 || read_command() == -1)
		return exit_state;	
		
	if(current_command == "logout")
	{
		_account = NULL;
		return initial;
	}
		
	const char* calc_cmd = "calc ";
	const size_t calc_length = strlen(calc_cmd);
	if(current_command.compare(0, calc_length, calc_cmd) != 0 || 
	   current_command.length() <= calc_length)
	{
		if(print("Invalid command\n") == -1)
			return exit_state;
		return handling;
	}
	
	return calc(current_command.substr(calc_length));
}

handler_state cmd_handler::calc(std::string calc_expression)
{
	
	int reserve_num = _account->reserve();
	if(reserve_num == -1)
	{
		if(print("Your account has insufficient funds\n") == -1)
			return exit_state;	
		return handling;
	}

	calc_node* node = calc_node::parse(calc_expression.c_str());
	if(node == NULL)
	{
		_account->free(reserve_num);
		if(print("Invalid expression\n") == -1)
			return exit_state;	
		return handling;
	}

	double result;	
	if(node->calc(result) == -1)
	{
		_account->free(reserve_num);
		delete node;
		if(print("Error calculating expression\n") == -1)
			return exit_state;	
		return handling;
	
	}
	
	char result_buf[1024];
	sprintf(result_buf, config::get_result_format(), result);
	if(_account->commit(reserve_num, calc_expression.c_str(), result_buf) == -1)
	{
		_account->free(reserve_num);
		delete node;
		print("Error in account system\n");
		return exit_state;
	}

	if(print(result_buf) == -1 || print("\n") == -1)
	{
		delete node;
		return exit_state;
	}

	delete node;
	return handling;
}

ip::tcp::socket& cmd_handler::get_socket()
{
	return _socket;
}

void cmd_handler::handler_proc(std::unique_ptr<cmd_handler>&& handler)
{
	try
	{
		handler->execute();
	}
	catch(const std::runtime_error& error)
	{
		log::log_error("Handler execution error: %s", error.what());
	}
}

void cmd_handler::start(std::unique_ptr<cmd_handler>&& handler)
{
	try
	{
		std::thread(cmd_handler::handler_proc, std::move(handler)).detach();
	}
	catch(const std::runtime_error& error)
	{
		log::log_error("Handler thread creation error: %s", error.what());
	}
}
