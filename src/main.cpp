#include <stdio.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <unistd.h>    
#include <signal.h>    
#include "config.h"
#include "log.h"
#include "listener.h"
#include "account.h"

int main(int argc, char *argv[])    
{    

	pid_t pid = fork();
	
	if(pid < 0)
	{
		log::log_errno("Error creating first child process");
		return -1;
	}
	
	if(pid > 0)
		return 0;
	
	if(setsid() < 0)
	{
		log::log_errno("Error creating session");
		return -1;
	}
	
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if(pid < 0)
	{
		log::log_errno("Error creating second child process");
		return -1;
	}
	
	if(pid > 0)
		return 0;
		
	umask(0);
	
	for(int closingFile = sysconf(_SC_OPEN_MAX); closingFile >= 0; closingFile--)
		close(closingFile);
			
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

	if (listener::start_v2() == -1)
	{         
		log::log_error("Error starting calc listener");
		return -1;
	}
		
	// This point is never reached.
	// Process is exited by cmd_handler when it accepts stop_key command
	return 0;
}
