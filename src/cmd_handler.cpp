#include <sys/types.h>    
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "cmd_handler.h"
#include "config.h"

cmd_handler::cmd_handler(int connfd)
{
	_connfd = connfd;
}

cmd_handler::~cmd_handler()
{
}

void cmd_handler::execute()
{

	handler_state state = initial;
	
	while(state != exit)
	{
		switch(state)
		{
			case initial:
				state = login();
				break;
			case handling:
				state = handle_command();
				break;	
			case exit:
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
				log::log_error("EOF reading user input");
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
	return print("calc>");
}

int cmd_handler::invalid_cmd()
{
	return print("Invalid command\n");
}

handler_state cmd_handler::login()
{
	
	if(hello() == -1 || wait() == -1 || read_command() == -1)
		return exit;	

	const char* login_cmd = "login ";
	const size_t login_length = strlen(login_cmd);
	if(current_command.compare(0, login_length, login_cmd) != 0 || 
	   current_command.length() <= login_length)
	{
		if(invalid_cmd() == -1)
			return exit;
		return initial;
	}
	
	std::string login_name = current_command.substr(login_length);
	
	if(hello() == -1 || wait() == -1 || read_command() == -1)
		return exit;	
	
	const char* pwd_cmd = "password ";
	const size_t pwd_length = strlen(pwd_cmd);
	if(current_command.compare(0, pwd_length, pwd_cmd) != 0 || 
	   current_command.length() <= pwd_length)
	{
		if(invalid_cmd() == -1)
			return exit;
		return initial;
	}
	
	std::string pwd = current_command.substr(pwd_length);
	
	log::log_console("Login: %s", login_name.c_str());
	log::log_console("Password: %s", pwd.c_str());

	return exit;
}

handler_state cmd_handler::handle_command()
{
	log::log_console("Handling commands");
	sleep(1);
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