#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>    
#include "config.h"

calc_user::calc_user(const char* name, const char* password)
{
	_name = name;
	_password = password;
}

const char* calc_user::get_name()
{
	return _name.c_str();
}

const char* calc_user::get_password()
{
	return _password.c_str();
}

std::string config::_host = "";
int config::_port = -1;
int config::_sessions_per_user = -1;
int config::_tests_per_session = -1;
std::vector<calc_user> config::_calc_users;

const char* config::get_host()
{
	return _host.c_str();
}

int config::get_port()
{
	return _port;
}

int config::get_sessions_per_user()
{
	return _sessions_per_user;
}

int config::get_tests_per_session()
{
	return _tests_per_session;
}

unsigned int config::get_users_count()
{
	return _calc_users.size();
}

const char* config::get_user_name(unsigned int user_index)
{
	if(user_index < 0 || user_index >= _calc_users.size())
		return NULL;
	return _calc_users[user_index].get_name();
}

const char* config::get_user_password(unsigned int user_index)
{
	if(user_index < 0 || user_index >= _calc_users.size())
		return NULL;
	return _calc_users[user_index].get_password();
}

int config::read()
{
	FILE* config_file = fopen("config.txt", "rt");
	if(config_file == NULL)
	{
		printf("Cannot open config file\n");
		return -1;
	}
	
	char buf[255];
	const char* user_setting = "user=";
	const char* port_setting = "port=";
	const char* host_setting = "host=";
	const char* sessions_per_user_setting = "sessions_per_user=";
	const char* tests_per_session_setting = "tests_per_session=";
	while(fgets(buf, 255, config_file) != NULL)
	{
		if(strncmp(buf, port_setting, strlen(port_setting)) == 0)
		{
			if(strlen(buf) > strlen(port_setting))
				_port = atoi(buf+strlen(port_setting));
		}
		else if(strncmp(buf, sessions_per_user_setting, strlen(sessions_per_user_setting)) == 0)
		{
			if(strlen(buf) > strlen(sessions_per_user_setting))	
				_sessions_per_user = atoi(buf+strlen(sessions_per_user_setting));
		}
		else if(strncmp(buf, tests_per_session_setting, strlen(tests_per_session_setting)) == 0)
		{
			if(strlen(buf) > strlen(tests_per_session_setting))	
				_tests_per_session = atoi(buf+strlen(tests_per_session_setting));
		}
		else if(strncmp(buf, host_setting, strlen(host_setting)) == 0)
		{
			if(strlen(buf) > strlen(host_setting))	
				_host = buf+strlen(host_setting);
		}
		else if(strncmp(buf, user_setting, strlen(user_setting)) == 0)
		{
			if(strlen(buf) > strlen(user_setting))
			{
				const char* separator_char = strchr(buf+strlen(user_setting), '/');
				if(separator_char == NULL)
				{
					printf("Invalid user setting: password not found\n");
					return -1;
				}
				
				_calc_users.push_back(calc_user(
					std::string(buf+strlen(user_setting), separator_char-buf-strlen(user_setting)).c_str(),
					std::string(separator_char+1).c_str()));

			}
		}
	}
	
	fclose(config_file);

	if(_port < 1024 || _port > 49151)
	{
		printf("Invalid port: %d (allowable values: 1024-49151)\n", _port);
		return -1;
	}
	
	if(_sessions_per_user < 1)
	{
		printf("Invalid sessions per user: %d (muset be at least 1)\n", _sessions_per_user);
		return -1;
	}
	
	if(_tests_per_session < 1)
	{
		printf("Invalid tests per session: %d (muset be at least 1)\n", _tests_per_session);
		return -1;
	}

	if(_host.empty())
	{
		printf("Host is empty\n");
		return -1;
	}
	
	if(_calc_users.size() == 0)
	{
		printf("No users defined\n");
		return -1;
	}

	return 0;
}