#include <stdio.h>
#include "calc_node.h"

calc_node* calc_node::parse(const char* calc_expression)
{

	calc_node* result = NULL;
	while(calc_expression[0] != '\0')
	{
		
		double number;	
		int	read_count = read_number(calc_expression, number);
		if(read_count == -1)
		{
			if(result != NULL)
				delete result;
			return NULL;
		}
		if(result == NULL)
			result = new calc_node(number);
		else
			result->accept_number(number);
	
		calc_expression += read_count;
		char operation;
		read_count = read_operation(calc_expression, operation);
		if(read_count == -1)
		{
			delete result;	
			return NULL;
		}
		else if(read_count > 0)
		{
			result = result->accept_operation(operation);
			calc_expression += read_count;
		}
	}
	
	if(result != NULL && !result->is_valid())
	{
		delete result;
		return NULL;
	}
	
	return result;
	
}

enum number_read_mode 
{
	before_dot,
	after_dot,
	exit
};

int calc_node::read_number(const char* calc_expression, double& result)
{
	
	int read_count = 0;
	while(calc_expression[read_count] == ' ')
		read_count++;
		
	result = 0;
	number_read_mode reading_mode = before_dot;
	double after_dot_mult = 0.1;
	int digit_status;
	bool has_digits = false;
	while(reading_mode != exit)
	{
		switch(calc_expression[read_count])
		{
			case '0':
				digit_status = 0;
				break;
			case '1':
				digit_status = 1;
				break;
			case '2':
				digit_status = 2;
				break;
			case '3':
				digit_status = 3;
				break;
			case '4':
				digit_status = 4;
				break;
			case '5':
				digit_status = 5;
				break;
			case '6':
				digit_status = 6;
				break;
			case '7':
				digit_status = 7;
				break;
			case '8':
				digit_status = 8;
				break;
			case '9':
				digit_status = 9;
				break;
			default:
				digit_status = -1;
				break;
		}
		
		if(digit_status >= 0)
		{
			has_digits = true;	
			read_count++;
			if(reading_mode == before_dot)
			{
				result = result * 10 + digit_status;
			}
			else
			{
				result = result + after_dot_mult * digit_status;
				after_dot_mult *= 0.1;
			}
		}
		else if(calc_expression[read_count] == '.')
		{
			reading_mode = after_dot;
			read_count++;
		}
		else
		{
			reading_mode = exit;
		}
	}
	
	return has_digits ? read_count : -1;
}

int calc_node::read_operation(const char* calc_expression, char& operation)
{

	int read_count = 0;
	while(calc_expression[read_count] == ' ')
		read_count++;

	switch(calc_expression[read_count])		
	{
		case '+':
		case '-':
		case '*':
		case '/':
			operation = calc_expression[read_count];
			read_count++;
			return read_count;
		case '\0':
			return 0;
		default:
			return -1;
	}
}

int calc_node::get_priority(char operation)
{
	if(operation == '*' || operation == '/')
		return 2;
	else
		return 1;
}

void calc_node::accept_number(double number)
{
	if(_right_operand == NULL)
		_right_operand = new calc_node(number);
	else
		_right_operand->accept_number(number);
}

calc_node* calc_node::accept_operation(char operation)
{
	if(_priority >= calc_node::get_priority(operation))
		return new calc_node(operation, this);

	_right_operand = _right_operand->accept_operation(operation);
	return this;
}

calc_node::calc_node(double number)
{
	_number = number;
	_priority = 3;
	_left_operand = NULL;
	_right_operand = NULL;
}

calc_node::calc_node(char operation, calc_node* left_operand)
{
	_left_operand = left_operand;
	_right_operand = NULL;
	_operation = operation;
	_priority = calc_node::get_priority(operation);
}

calc_node::~calc_node()
{
	if(_left_operand != NULL)
		delete _left_operand;
	if(_right_operand != NULL)
		delete _right_operand;
}

bool calc_node::is_valid()
{
	if(_priority == 3)	
		return true;
	return _left_operand != NULL && _right_operand != NULL && _left_operand->is_valid() && _right_operand->is_valid();
}

int calc_node::calc(double& result)
{
	if(_priority == 3)
	{
		result = _number;	
		return 0;
	}
	
	double left_result;
	if(_left_operand->calc(left_result) == -1)
		return -1;
	double right_result;
	if(_right_operand->calc(right_result) == -1)
		return -1;
	
	switch(_operation)
	{
		case '+':
			result = left_result + right_result;
			break;
		case '-':
			result = left_result - right_result;
			break;
		case '*':
			result = left_result * right_result;
			break;
		case '/':
			result = left_result / right_result;
			break;
	}	
	
	return 0;
}
