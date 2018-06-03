#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>    
#include "config.h"
#include "log.h"

int config::_port = -1;

int config::get_port()
{
	return _port;
}

int config::read()
{
	FILE* config_file = fopen("config.txt", "rt");
	if(config_file == NULL)
	{
		log::log_error("Cannot open config file");
		return -1;
	}
	
	char buf[100];
	const char* portSetting = "port=";
	while(fgets(buf, 100, config_file) != NULL)
	{
		if(strncmp(buf, portSetting, strlen(portSetting)) == 0 && 
		   strlen(buf) > strlen(portSetting))
		{
			_port = atoi(buf+strlen(portSetting));
		}
	}
	
	if(_port < 1024 || _port > 49151)
	{
		log::log_error("Invalid port: %d (allowable values: 1024-49151)", _port);
		return -1;
	}
	
	fclose(config_file);
	return 0;
	
}