#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>

void print_usage()
{
	printf("Usage:\n");
	printf(" calc_test user/password@host:port input_file output_file\n");
	printf("Example:\n");
	printf(" calc_test john/johnpwd@localhost:1500 commands.txt result.txt\n");
	printf(" commands.txt contents:\n");
	printf("  1+2*3\n");
	printf("  25-8/2+5\n");
	printf(" result.txt contents:\n");
	printf("  7\n");
	printf("  26\n");
}

int parse_connect(const char* connect_str, 
	std::string& user_name, std::string& user_pwd, std::string& host, int& port)
{
	const char* separator_char = strchr(connect_str, '/');
	if(separator_char == NULL)
	{
		printf("Password not found\n");
		return -1;
	}
	user_name = std::string(connect_str, separator_char - connect_str);
	if(user_name.empty())
	{
		printf("User name is empty\n");
		return -1;
	}
	
	const char* connect_str_next = separator_char + 1;
	separator_char = strchr(connect_str_next, '@');
	if(separator_char == NULL)
	{
		printf("Host not found\n");
		return -1;
	}
	user_pwd = std::string(connect_str_next, separator_char - connect_str_next);
	if(user_pwd.empty())
	{
		printf("Password is empty\n");
		return -1;
	}
		
	connect_str_next = separator_char + 1;
	separator_char = strchr(connect_str_next, ':');
	if(separator_char == NULL)
	{
		printf("Port not found\n");
		return -1;
	}
	host = std::string(connect_str_next, separator_char - connect_str_next);
	if(host.empty())
	{
		printf("Host is empty\n");
		return -1;
	}
		
	port = atoi(separator_char + 1);
	if(port < 1024 || port > 49151)
	{
		printf("Incorrect port: %d. Port must be between 1024 and 49151\n", port);
		return -1;
	}
	
	printf("user: %s\n", user_name.c_str());
	printf("password: %s\n", user_pwd.c_str());
	printf("host: %s\n", host.c_str());
	printf("port: %d\n", port);
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc != 4)
	{
		print_usage();	
		return 0;
	}

	std::string user_name, user_pwd, host;
	int port;
	if(parse_connect(argv[1], user_name, user_pwd, host, port) != 0)
		return -1;
		
	
	
	
}