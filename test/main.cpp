#include <stdlib.h>
#include <stdio.h>
#include "config.h"

int main(int argc, char *argv[])
{

	if(config::read() == -1)
		return -1;	

	return 0;

}