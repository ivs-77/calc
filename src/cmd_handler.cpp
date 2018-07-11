#include <sys/types.h>    
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include "log.h"
#include "cmd_handler.h"
#include "config.h"
#include "calc_node.h"

cmd_handler::cmd_handler(int connfd)
{
	_connfd = connfd;
	_account = NULL;
}

cmd_handler::~cmd_handler()
{
}

void cmd_handler::execute()
{

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
	
	close(_connfd);
}

int cmd_handler::wait()
{
	pollfd fd;
	fd.fd = _connfd;
	fd.events = POLLIN;
	fd.revents = 0;
	int retval = poll(&fd, 1, 1000 * config::get_login_timeout());
	if (retval == -1)
	{
		log::log_errno("Error while waiting for user input");
		return -1;
	}
	else if (retval)
	{
		return 0;
	}
	else
	{
		// timeout
		return -1;
	}
}

int cmd_handler::read_command()
{

	current_command.clear();
	
	char read_buffer[1025];
	read_buffer[1024] = 0;
	char* line_char	= NULL;
	while(line_char == NULL)
	{
		int current_pos = 0;
		while(line_char == NULL && current_pos < 1024)
		{
			int retval = read(_connfd, read_buffer + current_pos, 1024 - current_pos);
			if(retval == -1)
			{
				log::log_errno("Error reading user input");
				return -1;
			}
			else if(retval == 0)
			{
				// client closed connection
				return -1;
			}
			else
			{
				current_pos += retval;
				read_buffer[current_pos] = 0;
				line_char = strchr(read_buffer + current_pos - retval, '\n');
				if(line_char != NULL)
					line_char[0] = 0;
			}
		}
		current_command += read_buffer;
	}
	
	// for netcat -C mode: it ends line with \r\n
	if(current_command.length() > 0 && current_command[current_command.length() - 1] == '\r')
		current_command.resize(current_command.length() - 1);

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
	
	if(hello() == -1 || wait() == -1 || read_command() == -1)
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
	
	if(hello() == -1 || wait() == -1 || read_command() == -1)
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
	if(_account->commit(reserve_num, calc_expression, result_buf) == -1)
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

void* cmd_handler::handler_proc(void* data)
{
	cmd_handler* handler = (cmd_handler*)data;
	handler->execute();
	delete handler;
	return NULL;
}

void cmd_handler::start(int connfd)
{
	cmd_handler* handler = new cmd_handler(connfd);
	pthread_t handler_thread;
	int rc = pthread_create(&handler_thread, NULL, cmd_handler::handler_proc, (void*)handler);
	if(rc != 0)
	{
		delete handler;
		log::log_error("Handler thread creation error: %d", rc);
	}
}