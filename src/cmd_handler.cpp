#include <sys/types.h>    
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include "log.h"
#include "cmd_handler.h"
#include "config.h"

cmd_handler::cmd_handler(int connfd)
{
	_connfd = connfd;
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

handler_state cmd_handler::login()
{
	write(_connfd, "calc>", 5);	

	pollfd fd;
	fd.fd = _connfd;
	fd.events = POLLIN;
	fd.revents = 0;
	
	int retval = poll(&fd, 1, 1000 * config::get_login_timeout());

	if (retval == -1)
	{
		log::log_errno("Error while waiting for user input");
		return exit;
	}
	else if (retval)
	{
		char buf[101];
		char reading_char = 0;
		int read_count = 0;
		while(reading_char != '\n' && read_count < 100)
		{
			int read_res = read(_connfd, &reading_char, 1);
			log::log_console("Read: %c, res: %d", reading_char, read_res);
			buf[read_count++] = reading_char;
		}
		buf[read_count] = 0;
		log::log_console("Read: %s", buf);
		return exit;
	}
	else
	{
		// timeout
		return exit;
	}
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