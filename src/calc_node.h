#include <string>

class calc_node
{
public:
	int parse(std::string& calc_expression);
	int calc(double& result);

};