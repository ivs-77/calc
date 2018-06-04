#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>    
#include "config.h"
#include "log.h"

int config::_port = -1;
int config::_loginTimeout = 5;

int config::get_port()
{
	return _port;
}

int config::get_login_timeout()
{
	return _loginTimeout;
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
	const char* loginTimeoutSetting = "login_timeout=";
	while(fgets(buf, 100, config_file) != NULL)
	{
		if(strncmp(buf, portSetting, strlen(portSetting)) == 0)
		{
			if(strlen(buf) > strlen(portSetting))
				_port = atoi(buf+strlen(portSetting));
		}
		else if(strncmp(buf, loginTimeoutSetting, strlen(loginTimeoutSetting)) == 0)
		{
			if(strlen(buf) > strlen(loginTimeoutSetting))	
				_loginTimeout = atoi(buf+strlen(loginTimeoutSetting));
		}
	}
	
	if(_port < 1024 || _port > 49151)
	{
		log::log_error("Invalid port: %d (allowable values: 1024-49151)", _port);
		return -1;
	}
	
	if(_loginTimeout <= 0)
	{
		log::log_error("Invalid login timeout: %d (must be greater than 0)", _loginTimeout);
		return -1;
	}

	fclose(config_file);
	return 0;
	
}