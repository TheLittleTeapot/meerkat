#ifndef MEERKAT_SYSTEMS_CONSOLE_INPUT_H
#define MEERKAT_SYSTEMS_CONSOLE_INPUT_H

#include <systems/system.h>

#include <iostream>
#include <string>

#include <messaging/set_volume_request.h>

namespace systems
{
	class ConsoleInput : public System
	{
	public:
		ConsoleInput() = default;
		~ConsoleInput() = default;

	private:
		void threadUpdate() override
		{
			// Ouput a message
			std::cout << "Enter a command:" << std::endl;

			// get input
			std::string input;
			std::getline(std::cin, input);

			if(input == "test")
				m_listener->sendMessage(std::make_shared<messaging::SetVolumeRequest>(10));

			// Parse input to message
			
			// Send Message
		}

		void handleMessage(messaging::SharedMessage) override
		{

		}
	};
}

#endif // MEERKAT_SYSTEMS_CONSOLE_INPUT_H
