#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h> 
#include "config.h"
#include "log.h"
#include "cmd_handler.h"
#include "listener.h"

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

	log::log_info("Listener started. Accepting connections");
	while(true)
	{		
		int socket_conn = accept(list_socket, NULL, NULL);
		if(socket_conn >= 0)
			cmd_handler::start(socket_conn);
		else
			log::log_errno("Socket accept error");
	}
	
	// This point is never reached.
	// Process is exited by cmd_handler when it accepts stop_key command
	return 0;		
}