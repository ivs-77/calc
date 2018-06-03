#include <sys/types.h>    
#include <pthread.h>
#include <unistd.h>
#include "log.h"
#include "cmd_handler.h"

cmd_handler::cmd_handler(int connfd)
{
	_connfd = connfd;
}

void cmd_handler::execute()
{
	#define BUFLEN 1500    
	
	char buf[BUFLEN];
	ssize_t rcount = read(_connfd, buf, BUFLEN);
	if (rcount == -1) 
	{        
		log::log_errno("Socket read error");
	}    
	else
	{
		log::log_console("Socket read %ld bytes: %c\n", rcount, buf[0]);
	}
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
		log::log_error("Handler thread creation error: %d", rc);
}