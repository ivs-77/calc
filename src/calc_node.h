#include <string>

class calc_node
{
public:
	static calc_node* parse(const char* calc_expression);
	int calc(double& result);
	~calc_node();

private:
	static int read_number(const char* calc_expression, double& result);
	static int read_operation(const char* calc_expression, char& operation);
	static int get_priority(char operation);
	void accept_number(double number);
	calc_node* accept_operation(char operation);
	calc_node(double number);
	calc_node(char operation, calc_node* left_operand);
	bool is_valid();
	
	double _number;
	char _operation;
	int _priority;
	calc_node* _left_operand;
	calc_node* _right_operand;

};