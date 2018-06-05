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
	_command_buf = new char[config::get_max_cmd_length() + 1];
}

cmd_handler::~cmd_handler()
{
	delete _command_buf;
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
	int current_pos = 0;
	while(current_pos < config::get_max_cmd_length())
	{
		int retval = read(_connfd, _command_buf + current_pos, config::get_max_cmd_length() - current_pos);
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
			_command_buf[current_pos] = 0;
			char* line_char = strchr(_command_buf + current_pos - retval, '\n');
			if(line_char != NULL)
			{
				line_char[0] = 0;
				return 0;
			}
		}
	}
	log::log_error("User input is too long (max %d chars)", config::get_max_cmd_length());
	return -1;
}

void cmd_handler::hello()
{
	write(_connfd, "calc>", 5);	
}

handler_state cmd_handler::login()
{
	
	hello();	
	
	if(wait() == -1)
		return exit;
		
	if(read_command() == -1)
		return exit;
		
	log::log_console("Command: %s", _command_buf);
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