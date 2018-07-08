#include <unistd.h>
#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h> 
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include "calc_session.h"
#include "config.h"

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
		printf("Socket creation error\n");
		return;
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config::get_port());
	if(inet_pton(AF_INET, config::get_host(), &addr.sin_addr) <= 0)
	{
		printf("Inet_pton error\n");
		return;
	}
	
	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("Connect error\n");
		return;
	}

	
	char buf[1024];
	int n_read = read(sockfd, buf, 1023);
	if(n_read > 0)
	{
		buf[n_read] = 0;
		printf("user: %s, session: %d, read: %s\n", _user.c_str(), _session_index, buf);
	}
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
		printf("Session thread creation error: %d", rc);
		return -1;
	}
	return 0;
}