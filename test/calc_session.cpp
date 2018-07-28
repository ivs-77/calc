#include <unistd.h>
#include <sys/types.h>    
#include <sys/socket.h>    
#include <sys/time.h>
#include <netinet/in.h>    
#include <arpa/inet.h> 
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "calc_session.h"
#include "config.h"
#include "log.h"
#include "calc_node.h"

calc_session::calc_session(int session_index, const char* user, const char* password)
{
	_session_index = session_index;
	_user = user;
	_password = password;
	_calc_user_hello = user;
	_calc_user_hello += "@calc>";
	_total_session_calc_time = 0;
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
		if(make_test(sockfd) == -1)
		{
			close(sockfd);
			return;
		}
	}
	
	close(sockfd);
}

int calc_session::make_test(int sockfd)
{

	calc_node_first_level calc_node;
	calc_node.create();

	char buf[1000];
	calc_node.print(buf);
	
	char buf_res[100];
	calc_node.print_result(buf_res);
	
	if(read_str(sockfd, _calc_user_hello.c_str()) == -1)
		return -1;
	
	char test_buf[1000];
	sprintf(test_buf, "calc %s\n", buf);
	if(write_str(sockfd, test_buf) == -1)
		return -1;
	
	timeval calc_start_time;
	gettimeofday(&calc_start_time, NULL);
	std::string calc_res;
	if(read_line(sockfd, calc_res) == -1)
		return -1;
	timeval calc_end_time;
	gettimeofday(&calc_end_time, NULL);
	_total_session_calc_time += 
		(1.0 * calc_end_time.tv_sec + calc_end_time.tv_usec / 1e6) - 
		(1.0 * calc_start_time.tv_sec + calc_start_time.tv_usec / 1e6);
	
	if(strcmp(buf_res, calc_res.c_str()) == 0)
	{
		log::log_result(_user.c_str(), _session_index, buf, calc_res.c_str(), "OK");
	}
	else
	{
		char status_buf[100];
		sprintf(status_buf, "Fail. Exact answer: %s", buf_res);
		log::log_result(_user.c_str(), _session_index, buf, calc_res.c_str(), status_buf);
	}

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
	if(_line_rem.size() > 0)
	{
		strncpy(buf, _line_rem.c_str(), _line_rem.size());
		read_total = _line_rem.size();
		_line_rem.clear();
	}
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
		size_t current_pos = 0;
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
		if(line_str.size() < (current_pos - 1))
			_line_rem = std::string(line_char + 1, current_pos - line_str.size());
	}
	
	return 0;
}

void* calc_session::session_proc(void* data)
{
	log::session_started();
	calc_session* session_inst = (calc_session*)data;
	session_inst->execute();
	log::session_complete(session_inst->_total_session_calc_time);
	delete session_inst;
	return NULL;
}

int calc_session::start(int session_index, const char* user, const char* password)
{
	calc_session* session_inst = new calc_session(session_index, user, password);
	pthread_t thread;
	int rc = pthread_create(&thread, NULL, calc_session::session_proc, (void*)session_inst);
	if(rc != 0)
	{
		delete session_inst;
		log::log_console("Session thread creation error: %d", rc);
		return -1;
	}
	return 0;
}