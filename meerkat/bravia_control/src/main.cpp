#include <iostream>
#include <string>

#include "bravia_controller.h"


int main(void)
{
	bravia::Controller bravia("192.168.1.116:20060");

	bravia.setIrccCode(30, [](bool res)
	{
		printf("result of the code was: %s\n", res == 1 ? "true" : "false"); 
	});

	for (;;)
	{
		bravia.update();
	}
}