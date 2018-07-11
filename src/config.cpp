#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>    
#include "config.h"
#include "log.h"

int config::_port = -1;
int config::_loginTimeout = 5;
std::string config::_connect;
std::string config::_stop_key;
std::string config::_result_format = "%14.3f";

const char* config::get_result_format()
{
	return _result_format.c_str();
}

int config::get_port()
{
	return _port;
}

int config::get_login_timeout()
{
	return _loginTimeout;
}

const char* config::get_connect()
{
	return _connect.c_str();
}

const char* config::get_stop_key()
{
	return _stop_key.c_str();
}

int config::read()
{
	FILE* config_file = fopen("config.txt", "rt");
	if(config_file == NULL)
	{
		log::log_error("Cannot open config file");
		return -1;
	}
	
	char buf[255];
	const char* portSetting = "port=";
	const char* loginTimeoutSetting = "login_timeout=";
	const char* connectSetting = "connect=";
	const char* stopKeySetting = "stop_key=";
	const char* resultFormatSetting = "result_format=";
	while(fgets(buf, 255, config_file) != NULL)
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
		else if(strncmp(buf, connectSetting, strlen(connectSetting)) == 0)
		{
			if(strlen(buf) > strlen(connectSetting))
			{	
				_connect = buf+strlen(connectSetting);
				if(_connect.length() > 0 && _connect[_connect.length() - 1] == '\n')
					_connect.resize(_connect.length() - 1);
			}
		}
		else if(strncmp(buf, stopKeySetting, strlen(stopKeySetting)) == 0)
		{
			if(strlen(buf) > strlen(stopKeySetting))	
			{
				_stop_key = buf+strlen(stopKeySetting);
				if(_stop_key.length() > 0 && _stop_key[_stop_key.length() - 1] == '\n')
					_stop_key.resize(_stop_key.length() - 1);
			}
		}
		else if(strncmp(buf, resultFormatSetting, strlen(resultFormatSetting)) == 0)
		{
			if(strlen(buf) > strlen(resultFormatSetting))	
			{
				_result_format = buf+strlen(resultFormatSetting);
				if(_result_format.length() > 0 && _result_format[_result_format.length() - 1] == '\n')
					_result_format.resize(_result_format.length() - 1);
			}
		}
	}
	
	fclose(config_file);

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

	if(_connect.empty())
	{
		log::log_error("Connect is empty");
		return -1;
	}

	if(_stop_key.empty())
	{
		log::log_error("Stop key is empty");
		return -1;
	}

	if(_result_format.empty())
	{
		log::log_error("Result format is empty");
		return -1;
	}

	return 0;
}