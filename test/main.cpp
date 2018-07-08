#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "config.h"
#include "calc_session.h"

int main(int argc, char *argv[])
{

	if(config::read() == -1)
		return -1;	
		
	const int total_sessions_count = config::get_users_count() * config::get_sessions_per_user();
	pthread_t session_threads[total_sessions_count];
	for(unsigned int user_index = 0; user_index < config::get_users_count(); user_index++)
	for(int session_index = 0; session_index < config::get_sessions_per_user(); session_index++)
	{
		if(calc_session::start(session_index,
				config::get_user_name(user_index), 
				config::get_user_password(user_index),
				&(session_threads[user_index*config::get_sessions_per_user() + session_index])) != 0)
		{
			return -1;
		}
	}
	
	for(int total_session_index = 0; total_session_index < total_sessions_count; total_session_index++)
		pthread_join(session_threads[total_session_index], NULL);

	return 0;

}