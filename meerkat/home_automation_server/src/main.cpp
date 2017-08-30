#include <systems/console_input.h>
#include <systems/bravia_control.h>
#include <systems/systems_monitor.h>
#include <messaging/message_dispatcher.h>

int main(void)
{
	auto dispatcher = std::make_shared<messaging::MessageDispatcher>();

	systems::SystemsMonitor systemsMonitor;
	systems::ConsoleInput	consoleInputSystem;
	systems::BraviaControl	braviaControlSystem;

	consoleInputSystem.init(std::move(dispatcher->createListener({ })));
	braviaControlSystem.init(std::move(dispatcher->createListener({ messaging::Type::Set_Volume_Request, messaging::Type::Get_Volume_Request })));
	systemsMonitor.init(std::move(dispatcher->createListener({ messaging::Type::Set_Volume_Response, messaging::Type::Set_Volume_Request })));

	for (;;)
	{
	}

	systemsMonitor.term();
	consoleInputSystem.term();
	braviaControlSystem.term();
}