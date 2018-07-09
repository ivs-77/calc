#include <unistd.h>
#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h> 
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "calc_session.h"
#include "config.h"
#include "log.h"

calc_session::calc_session(int session_index, const char* user, const char* password)
{
	_session_index = session_index;
	_user = user;
	_password = password;
}

calc_session::~calc_session()
{
}

void calc_session::execute()
{
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		log::log_console("Socket creation error");
		return;
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config::get_port());
	if(inet_pton(AF_INET, config::get_host(), &addr.sin_addr) <= 0)
	{
		log::log_console("Inet_pton error");
		return;
	}
	
	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		log::log_console("Connect error");
		return;
	}
	
	if(login(sockfd) == -1)
	{
		close(sockfd);
		return;
	}
	
	for(int test_index = 0; test_index < config::get_tests_per_session(); test_index++)
	{
		if(make_test(sockfd, test_index) == -1)
		{
			close(sockfd);
			return;
		}
	}
}

int calc_session::make_test(int sockfd, int test_index)
{

	double first_arg = (rand() % 1000 + 1) + (rand() % 1000) / 1000.0;
	double second_arg = (rand() % 1000 + 1) + (rand() % 1000) / 1000.0;
	
	int oper = rand() % 4;
	char oper_char;

	if(oper == 0)
		oper_char = '*';
	else if(oper == 1)
		oper_char = '/';
	else if(oper == 2)
		oper_char = '+';
	else 
		oper_char = '-';
		
	char buf[1024];
	sprintf(buf, "%f %c %f", first_arg, oper_char, second_arg);
	
	std::string test_str = "calc ";
	test_str += buf;
	test_str += "\n";
	if(write_str(sockfd, test_str.c_str()) == -1)
		return -1;
	
	std::string out_line;
	if(read_line(sockfd, out_line) == -1)
		return -1;
	
	log::log_console("Test: %s, result: %s", buf, out_line.c_str());

	return 0;
}

int calc_session::login(int sockfd)
{

	if(read_str(sockfd, "calc>") == -1)
		return -1;
		
	std::string login_str = "login ";
	login_str += _user;
	login_str += "\n";
	
	if(write_str(sockfd, login_str.c_str()) == -1)
		return -1;

	if(read_str(sockfd, "calc>") == -1)
		return -1;
	
	login_str = "password ";
	login_str += _password;
	login_str += "\n";
	
	if(write_str(sockfd, login_str.c_str()) == -1)
		return -1;
	
	std::string calc_str = _user;
	calc_str += "@calc>";

	if(read_str(sockfd, calc_str.c_str()) == -1)
		return -1;
	
	return 0;
}

int calc_session::write_str(int sockfd, const char* str)
{
	size_t result = write(sockfd, str, strlen(str));
	if(result < strlen(str))
	{
		log::log_console("Error writing to calc input: %s", str);
		return -1;
	}
	return 0;
}

int calc_session::read_str(int sockfd, const char* str)
{
	char buf[strlen(str)];
	size_t read_total = 0;
	while(read_total < strlen(str))
	{
		int read_count = read(sockfd, buf + read_total, strlen(str)-read_total);
		if(read_count < 1)
		{
			log::log_console("No calc output to read");
			return -1;
		}
		read_total += read_count;
	}
	if(strncmp(buf, str, strlen(str)) != 0)
	{
		log::log_console("Calc output is not equal to: %s", str);
		return -1;
	}
	return 0;
}

int calc_session::read_line(int sockfd, std::string& line_str)
{
	
	line_str.clear();
	
	char read_buffer[1025];
	read_buffer[1024] = 0;
	char* line_char	= NULL;
	while(line_char == NULL)
	{
		int current_pos = 0;
		while(line_char == NULL && current_pos < 1024)
		{
			int retval = read(sockfd, read_buffer + current_pos, 1024 - current_pos);
			if(retval < 1)
			{
				log::log_console("No calc output to read");
				return -1;
			}
			else
			{
				current_pos += retval;
				read_buffer[current_pos] = 0;
				line_char = strchr(read_buffer + current_pos - retval, '\n');
				if(line_char != NULL)
					line_char[0] = 0;
			}
		}
		line_str += read_buffer;
	}
	
	return 0;
}

void* calc_session::session_proc(void* data)
{
	calc_session* session_inst = (calc_session*)data;
	session_inst->execute();
	delete session_inst;
	return NULL;
}

int calc_session::start(int session_index, const char* user, const char* password, pthread_t* session_thread)
{
	calc_session* session_inst = new calc_session(session_index, user, password);
	int rc = pthread_create(session_thread, NULL, calc_session::session_proc, (void*)session_inst);
	if(rc != 0)
	{
		delete session_inst;
		log::log_console("Session thread creation error: %d", rc);
		return -1;
	}
	return 0;
}