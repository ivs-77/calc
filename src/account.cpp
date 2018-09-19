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
	if(_conn != NULL)
	{
		PQfinish(_conn);
		_conn = NULL;
	}
	pthread_mutex_destroy(&_account_mutex);
}

account::account(int id, const char* name, const char* pwd, int amount)
{
	_id = id;
	_name = name;
	_pwd = pwd;
	_amount = amount;
	_max_reserved = 0;
	_conn = NULL;
	pthread_mutex_init(&_account_mutex, NULL);
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
	pthread_mutex_lock(&_account_mutex);

	if(_amount == 0 || _reserved.size() == _amount)
	{
		pthread_mutex_unlock(&_account_mutex);
		return -1;
	}	

	int result;
	if(_free.empty())
	{
		result = _max_reserved++;
	}
	else
	{
		result = _free.top();
		_free.pop();
	}
	_reserved.insert(result);
	pthread_mutex_unlock(&_account_mutex);
	return result;
}

void account::free(int reserv_num)
{
	pthread_mutex_lock(&_account_mutex);
	std::set<int>::iterator reserved_iter = _reserved.find(reserv_num);
	if(reserved_iter == _reserved.end())
	{
		log::log_error("Invalid reserve number");
		pthread_mutex_unlock(&_account_mutex);
		return;
	}
	_reserved.erase(reserved_iter);
	_free.push(reserv_num);
	pthread_mutex_unlock(&_account_mutex);
}

int account::commit(int reserv_num, const char* calc_expression, const char* result)
{
	pthread_mutex_lock(&_account_mutex);
	std::set<int>::iterator reserved_iter = _reserved.find(reserv_num);
	if(reserved_iter == _reserved.end())
	{
		log::log_error("Invalid reserve number");
		pthread_mutex_unlock(&_account_mutex);
		return -1;
	}
	
	if(_conn == NULL)
		_conn = PQconnectdb(config::get_connect());

	if (PQstatus(_conn) != CONNECTION_OK)
	{        
		log::log_error("Connection to database failed: %s", PQerrorMessage(_conn));
	 	PQfinish(_conn);
		_conn = NULL;
		pthread_mutex_unlock(&_account_mutex);
		return -1;
	}
	
	PGresult *res = PQexec(_conn, "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{        
		log::log_error("Start transaction error. Error: %s", PQresultErrorMessage(res));
		PQclear(res);
		pthread_mutex_unlock(&_account_mutex);
		return -1;
	}

	char id_str[100];
	sprintf(id_str, "%d", _id);
	
	const char* param_values[3];
	param_values[0] = id_str;
	param_values[1] = calc_expression;
	param_values[2] = result;
	
	res = PQexecParams(_conn, 
		"insert into account_log(account_id, expr, res) values (cast($1 as integer), $2, $3)",
		3, NULL, param_values, NULL, NULL, 0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{        
		log::log_error("Insert into account_log error. Error: %s", PQresultErrorMessage(res));
		PQexec(_conn, "ROLLBACK");
		PQclear(res);
		pthread_mutex_unlock(&_account_mutex);
		return -1;
	}
	
	res = PQexecParams(_conn, 
		"update accounts set amount = amount - 1 where id = cast($1 as integer)",
		1, NULL, param_values, NULL, NULL, 0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{        
		log::log_error("Update account error. Error: %s", PQresultErrorMessage(res));
		PQexec(_conn, "ROLLBACK");
		PQclear(res);
		pthread_mutex_unlock(&_account_mutex);
		return -1;
	}

	res = PQexec(_conn, "COMMIT");
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{        
		log::log_error("Commit error. Error: %s", PQresultErrorMessage(res));
		PQclear(res);
		pthread_mutex_unlock(&_account_mutex);
		return -1;
	}

	PQclear(res);
	
	_amount--;
	_reserved.erase(reserved_iter);
	_free.push(reserv_num);
	pthread_mutex_unlock(&_account_mutex);
	return 0;
}
