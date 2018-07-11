#include <stdio.h>    
#include <stdlib.h>    
#include "calc_node.h"

void calc_node_first_level::create()
{
	first_arg.create();	
	first_oper = (rand() % 2) == 1 ? '+' : '-';
	second_arg.create();
	second_oper = (rand() % 2) == 1 ? '+' : '-';
	third_arg.create();
}

void calc_node_first_level::print(char* buf)
{
	char first_buf[100];
	first_arg.print(first_buf);
	char second_buf[100];
	second_arg.print(second_buf);
	char third_buf[100];
	third_arg.print(third_buf);
	sprintf(buf, "%s%c%s%c%s", first_buf, first_oper, second_buf, second_oper, third_buf);
}

void calc_node_first_level::print_result(char* buf)
{
	double res = (first_oper == '+') ? 
		(first_arg.get_result() + second_arg.get_result()) :
		(first_arg.get_result() - second_arg.get_result());
	
	res = (second_oper == '+') ? 
		res + third_arg.get_result() :
		res - third_arg.get_result();
		
	sprintf(buf, "%14.3f", res);
}

void calc_node_second_level::create()
{
	first_arg = (rand() % 999 + 1) + (rand() % 1000) / 1000.0;
	first_oper = (rand() % 2) == 1 ? '*' : '/';
	second_arg = (rand() % 999 + 1) + (rand() % 1000) / 1000.0;
	second_oper = (rand() % 2) == 1 ? '*' : '/';
	third_arg  = (rand() % 999 + 1) + (rand() % 1000) / 1000.0;
}

void calc_node_second_level::print(char* buf)
{
	sprintf(buf, "%7.3f%c%7.3f%c%7.3f", first_arg, first_oper, second_arg, second_oper, third_arg);
}

double calc_node_second_level::get_result()
{
	double res = (first_oper == '*') ? (first_arg *  second_arg) : (first_arg / second_arg);
	return (second_oper == '*') ? (res * third_arg) : (res / third_arg);
}

