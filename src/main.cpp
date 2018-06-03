#include <stdio.h>    
#include "log.h"
#include "listener.h"

int main(int argc, char *argv[])    
{    
	if (listener::start(1500, 20) == -1)
	{         
		log::log_console("Error starting calc listener. See error log for details.");
		return -1;
	}
	
	log::log_console("Calc listener started. Press Enter to stop.");
	char c[3];
	fgets(c, 2, stdin);
}
