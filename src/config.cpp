#include <boost/program_options.hpp>
#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>    
#include <fstream>
#include "config.h"
#include "log.h"

using namespace  boost::program_options;

int config::_port;
int config::_login_timeout;
std::string config::_connect;
std::string config::_stop_key;
std::string config::_result_format;

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
	return _login_timeout;
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


	std::ifstream config_file("config.txt");
	if(!config_file)
	{
		log::log_error("Cannot open config file");
		return -1;
	}

	try
	{
		options_description file_options("config_file");
		file_options.add_options()
			("port", value(&_port)->default_value(1500))
			("login_timeout", value(&_login_timeout)->default_value(30))
			("connect", value(&_connect))
			("stop_key", value(&_stop_key))
			("result_format", value(&_result_format)->default_value("%14.3f"));
		variables_map config_options;
		store(parse_config_file(config_file, file_options), config_options);
		notify(config_options);
	
	}
	catch(const std::exception& error)
	{
		log::log_error("Error reading config file: %s", error.what());
		return -1;
	}
	
	if(_port < 1024 || _port > 49151)
	{
		log::log_error("Invalid port: %d (allowable values: 1024-49151)", _port);
		return -1;
	}
	
	if(_login_timeout < 1)
	{
		log::log_error("Invalid login timeout: %d (must be at least 1)", _login_timeout);
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