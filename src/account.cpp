#include <libpq-fe.h>
#include "log.h"
#include "config.h"
#include "account.h"

int account::load_accounts()
{
	PGconn *conn = PQconnectdb(config::get_connect());
	
	if (PQstatus(conn) != CONNECTION_OK)    
	{        
		log::log_error("Connection to database failed: %s", PQerrorMessage(conn));
	    PQfinish(conn);
	    return -1;
	}
	
	log::log_console("Connected");
	return 0;
}

account* account::get_account(const char* login, const char* pwd)
{
	return NULL;	
}