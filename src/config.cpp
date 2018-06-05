#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>    
#include "config.h"
#include "log.h"

int config::_port = -1;
int config::_loginTimeout = 5;
int config::_max_cmd_length = 1024;

int config::get_port()
{
	return _port;
}

int config::get_login_timeout()
{
	return _loginTimeout;
}

int config::get_max_cmd_length()
{
	return _max_cmd_length;
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
	const char* maxCmdLengthSetting = "max_cmd_length=";
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
		else if(strncmp(buf, maxCmdLengthSetting, strlen(maxCmdLengthSetting)) == 0)
		{
			if(strlen(buf) > strlen(maxCmdLengthSetting))	
				_max_cmd_length = atoi(buf+strlen(maxCmdLengthSetting));
		}
	}
	
	if(_port < 1024 || _port > 49151)
	{
		log::log_error("Invalid port: %d (allowable values: 1024-49151)", _port);
		return -1;
	}
	
	if(_loginTimeout < 1)
	{
		log::log_error("Invalid login timeout: %d (muset be at least 1)", _loginTimeout);
		return -1;
	}

	if(_max_cmd_length < 1024)
	{
		log::log_error("Invalid max command length: %d (must be at least 1024)", _max_cmd_length);
		return -1;
	}

	fclose(config_file);
	return 0;
	
}