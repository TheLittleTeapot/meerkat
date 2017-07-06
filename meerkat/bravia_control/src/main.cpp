#include "../include/tcp_client.h"
#include "../include/mg_manager.h"

#include <iostream>
#include <string>

int main(void)
{
	Mongoose::Manager mgr;

	const auto& wkBravia = mgr.connectWeak("127.0.0.1:1234");

	/*const auto clientConnection = mgr.connect("127.0.0.1:1234");
	if (clientConnection)
		clientConnection->send("Hello world!");
*/
	mgr.poll( std::chrono::milliseconds(10) );

	if (auto sharedBraviaConnection = wkBravia.lock())
	{
		sharedBraviaConnection->send("HI\nThe weak pointer could be locked!");
	}

}