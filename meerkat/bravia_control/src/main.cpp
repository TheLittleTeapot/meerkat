#include "bravia_controller.h"

#include <iostream>
#include <string>
#include <queue>
#include <chrono>

int main(void)
{
	using irc = bravia::command::IrCode;

	//bravia::Controller bravia("192.168.1.116:20060");
	bravia::Controller bravia("192.168.1.4:20060");

	bravia.setIrccCode(irc::Volume_Up, [](bool res)
	{
		printf("result of the code was: %s\n", res == 1 ? "true" : "false"); 
	});

	std::queue<bravia::command::IrCode> codesToExecute;

	codesToExecute.emplace(irc::Netflix);
	codesToExecute.emplace(irc::Volume_Down);
	codesToExecute.emplace(irc::Volume_Up);
	codesToExecute.emplace(irc::Confirm);

	auto then = std::chrono::high_resolution_clock::now();
	for (;;)
	{
		auto now = std::chrono::high_resolution_clock::now();

		if (std::chrono::duration_cast<std::chrono::seconds>(now - then).count() > 10)
		{
			if (codesToExecute.empty())
				break;

			const auto code = codesToExecute.front();
			codesToExecute.pop();

			bravia.setIrccCode(code, [](bool res)
			{
				printf("result of the code was: %s\n", res == 1 ? "true" : "false");
			});

			then = std::chrono::high_resolution_clock::now();
		}

		bravia.update();
	}
}