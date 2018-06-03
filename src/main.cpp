#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>    
#include <stdio.h>    
#include <errno.h>    
#include <stdlib.h>    
#include <string.h>    
#include <unistd.h>
#include "log.h"

int main(int argc, char *argv[])    
{    
	#define BUFLEN 1500    
	
	fopen("123.txt", "rb");
	log::log_error("Это ошибка");
	errno = 0;
	log::log_error("Это другая ошибка");
	return 0;

	int fd;    
	ssize_t i;    
	ssize_t rcount;    
	char buf[BUFLEN];    
	printf("Calc started\n");  
	fd = socket(AF_INET, SOCK_STREAM, 0);    
	if (fd == -1)
	{         
		printf("Socket creation error: %s\n", strerror(errno)); 
		return -1;
	}
	else
	{
		printf("Socket created\n"); 
	}    
	
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1500);
	
	
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) 
	{        
		printf("Socket bind error: %s\n", strerror(errno)); 
		return -1;
	}    
	else
	{
		printf("Socket binded\n"); 
	}
	
	if (listen(fd, 20) == -1) 
	{        
		printf("Socket listen error: %s\n", strerror(errno)); 
		return -1;
	}    
	else
	{
		printf("Socket is listening\n"); 
	}
	
	
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen = sizeof(cliaddr);
	connfd = accept(fd, (struct sockaddr *) &cliaddr, &cliaddrlen);    
	if (connfd == -1) 
	{        
		printf("Socket accept error: %s\n", strerror(errno)); 
		return -1;
	}    
	else
	{
		printf("Socket connection is accepted\n"); 
	}
	
	rcount = read(connfd, buf, BUFLEN);
	if (rcount == -1) 
	{        
		printf("Socket read error: %s\n", strerror(errno)); 
		return -1;
	}    
	else
	{
		printf("Socket read %ld bytes:\n", rcount); 
	}
	
	for (i = 0; i < rcount; i++) 
	{
		printf("%c", buf[i]);
	}    

}
