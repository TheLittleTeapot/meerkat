#include <systems/console_input.h>
#include <systems/bravia_control.h>
#include <messaging/message_dispatcher.h>

int main(void)
{
	auto dispatcher = std::make_shared<messaging::MessageDispatcher>();

	systems::ConsoleInput	consoleInputSystem;
	systems::BraviaControl	braviaControlSystem;

	consoleInputSystem.init(std::move(dispatcher->createListener({ messaging::Type::Set_Volume_Response })));
	braviaControlSystem.init(std::move(dispatcher->createListener({ messaging::Type::Set_Volume_Request })));
	
	for (;;)
	{
	}

	consoleInputSystem.term();
	braviaControlSystem.term();
}