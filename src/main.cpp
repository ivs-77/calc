#include <stdio.h>    
#include <sys/types.h>    
#include <unistd.h>    
#include "config.h"
#include "log.h"
#include "listener.h"
#include "account.h"

int main(int argc, char *argv[])    
{    

	pid_t pid = fork();
	
	if(pid < 0)
	{
		log::log_error("Error creating child process");
		return -1;
	}
	
	if (pid == 0)
	{

		if (config::read() == -1)
		{         
			log::log_error("Error reading calc config");
			return -1;
		}

		if (account::load_accounts() == -1)
		{         
			log::log_error("Error loading accounts");
			return -1;
		}

		if (listener::start() == -1)
		{         
			log::log_error("Error starting calc listener");
			return -1;
		}
		
		// This point is never reached.
		// Process is exited by cmd_handler when it accepts stop_key command
		return 0;
	}
	else
	{
		log::log_info("Calc started");
		return 0;
	}
}
