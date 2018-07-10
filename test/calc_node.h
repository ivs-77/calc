
class calc_node_second_level
{
public:

	void create();
	void print(char* buf);
	double get_result();

private:

	double first_arg;
	char first_oper;
	double second_arg;
	char second_oper;
	double third_arg;

};

class calc_node_first_level
{
public:

	void create();
	void print(char* buf);
	void print_result(char* buf);

private:

	calc_node_second_level first_arg;	
	char first_oper;
	calc_node_second_level second_arg;
	char second_oper;
	calc_node_second_level third_arg;

};


