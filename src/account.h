#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <stack>
#include <libpq-fe.h>

class account;

typedef std::unique_ptr<account> account_ptr;

class accounts
{
public:
	
	accounts();

	void add(int id, const char* name, const char* pwd, int amount);
	account* get_account(const std::string& login, const std::string& pwd);
	
private:

	std::map<std::string, account_ptr> _accounts;

};

class account
{
public:
	static int load_accounts();
	static account* get_account(const std::string& login, const std::string& pwd);
	bool pwd_equals(const std::string& pwd);
	int reserve();
	void free(int reserv_num);
	int commit(int reserv_num, const char* calc_expression, const char* result);
	const char* get_name();
	account(int id, const char* name, const char* pwd, int amount);
	~account();

private:
	int _id;
	std::string _name;
	std::string _pwd;
	unsigned int  _amount;
	
	std::set<int> _reserved;
	std::stack<int> _free;
	int _max_reserved;
	std::mutex _account_mutex;
	PGconn* _conn;
	
	static accounts _accounts;
};

