#include <libpq-fe.h>
#include <stdlib.h>
#include "log.h"
#include "config.h"
#include "account.h"

accounts::accounts()
{
}

accounts::~accounts()
{
	while(!_accounts.empty())
	{
		delete _accounts.begin()->second;
		_accounts.erase(_accounts.begin());
	}
}

void accounts::add(int id, const char* name, const char* pwd, int amount)
{
	_accounts.insert(std::pair<std::string, account*>(name, 
		new account(id, name, pwd, amount)));
}
	
account* accounts::get_account(const std::string& login, const std::string& pwd)
{
	std::map<std::string, account* >::iterator login_account = _accounts.find(login);
	if(login_account == _accounts.end())
		return NULL;
	
	return login_account->second->pwd_equals(pwd) ? login_account->second : NULL;
}

accounts account::_accounts;

account::~account()
{
}

account::account(int id, const char* name, const char* pwd, int amount)
{
	_id = id;
	_name = name;
	_pwd = pwd;
	_amount = amount;
}

bool account::pwd_equals(const std::string& pwd)
{
	return _pwd == pwd;
}

const char* account::get_name()
{
	return _name.c_str();
}

int account::load_accounts()
{
	PGconn *conn = PQconnectdb(config::get_connect());
	
	if (PQstatus(conn) != CONNECTION_OK)    
	{        
		log::log_error("Connection to database failed: %s", PQerrorMessage(conn));
	    PQfinish(conn);
	    return -1;
	}
	
	PGresult *res = PQexec(conn, "select id, name, pwd, amount from accounts");
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{        
		log::log_error("No accounts found. Error: %s", PQresultErrorMessage(res));
	    PQclear(res);
		PQfinish(conn);
	    return -1;
	}
	
	for(int row_number = 0; row_number < PQntuples(res); row_number++)
	{
		_accounts.add(	
			atoi(PQgetvalue(res, row_number, 0)),
			PQgetvalue(res, row_number, 1),
			PQgetvalue(res, row_number, 2),
			atoi(PQgetvalue(res, row_number, 3)));
	}
	
    PQclear(res);
	PQfinish(conn);
	return 0;
}

account* account::get_account(const std::string& login, const std::string& pwd)
{
	return _accounts.get_account(login, pwd);
}

int account::reserve()
{
	return -1;
}

void account::free(int reserv_num)
{
}

int account::commit(int reserv_num, std::string& calc_expression, double result)
{
	return -1;
}
