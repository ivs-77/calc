#include <map>
#include <set>
#include <stack>

class account;

class accounts
{
public:
	
	accounts();
	~accounts();

	void add(int id, const char* name, const char* pwd, int amount);
	account* get_account(const std::string& login, const std::string& pwd);
	
private:

	std::map< std::string, account* > _accounts;

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
	pthread_mutex_t _account_mutex;
	
	static accounts _accounts;
};

