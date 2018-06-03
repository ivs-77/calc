#include <stdio.h>    
#include "config.h"
#include "log.h"
#include "listener.h"

int main(int argc, char *argv[])    
{    
	if (config::read() == -1)
	{         
		log::log_console("Error reading calc config. See error log for details.");
		return -1;
	}

	if (listener::start(config::get_port()) == -1)
	{         
		log::log_console("Error starting calc listener. See error log for details.");
		return -1;
	}
	
	log::log_console("Calc listener started. Press Enter to stop.");
	char c[2];
	fgets(c, 2, stdin);
}
