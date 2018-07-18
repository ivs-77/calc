#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "config.h"
#include "calc_session.h"
#include "log.h"

int main(int argc, char *argv[])
{

	if(config::read() == -1)
		return -1;	
		
	for(unsigned int user_index = 0; user_index < config::get_users_count(); user_index++)
	for(int session_index = 0; session_index < config::get_sessions_per_user(); session_index++)
	{
		if(calc_session::start(session_index,
				config::get_user_name(user_index), 
				config::get_user_password(user_index)) != 0)
		{
			return -1;
		}
	}
	
	while(!log::all_sessions_complete())
	{
		log::log_percent();
		sleep(1);
	}
	log::log_percent();
	fflush(stdout);
	
	return 0;

}