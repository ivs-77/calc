#include <map>

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
	const char* get_name();
	account(int id, const char* name, const char* pwd, int amount);
	~account();

private:
	int _id;
	std::string _name;
	std::string _pwd;
	int  _amount;
	
	static accounts _accounts;
};

