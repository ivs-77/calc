#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h> 
#include <pthread.h>
#include "config.h"
#include "log.h"
#include "cmd_handler.h"
#include "listener.h"

listener::listener(int list_socket)
{
	_list_socket = list_socket;
}

void listener::execute()
{
	while(true)
	{		
		int socket_conn = accept(_list_socket, NULL, NULL);
		if(socket_conn >= 0)
			cmd_handler::start(socket_conn);
		else
			log::log_errno("Socket accept error");
	}
}

void* listener::listener_proc(void* data)
{
	listener* listener_inst = (listener*)data;
	listener_inst->execute();
	delete listener_inst;
	return NULL;
}

int listener::start()
{
	int list_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (list_socket == -1)
	{         
		log::log_errno("Socket creation error");
		return -1;
	}
	
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config::get_port());
	
	if (bind(list_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) 
	{        
		log::log_errno("Socket bind error");
		return -1;
	}    
	
	if (listen(list_socket, SOMAXCONN) == -1) 
	{        
		log::log_errno("Socket listen error");
		return -1;
	}    

	listener* listener_inst = new listener(list_socket);
	pthread_t handler_thread;
	int rc = pthread_create(&handler_thread, NULL, listener::listener_proc, (void*)listener_inst);
	if(rc != 0)
	{
		delete listener_inst;
		log::log_error("Listener thread creation error: %d", rc);
		return -1;
	}
	
	return 0;		
}