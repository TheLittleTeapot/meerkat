#ifndef MEERKAT_SYSTEMS_CONSOLE_INPUT_H
#define MEERKAT_SYSTEMS_CONSOLE_INPUT_H

#include <systems/system.h>

#include <iostream>
#include <string>

#include <messaging/set_volume_request.h>
#include <messaging/set_volume_response.h>

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

			if(!input.empty())
				m_listener->sendMessage(std::make_shared<messaging::SetVolumeRequest>(std::stoi(input)));

			// Parse input to message
			
			// Send Message
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

		void handleMessage(messaging::SharedMessage message) override
		{
			auto t = message->getType();

			switch (t)
			{
			case messaging::Type::Set_Volume_Response:
			{
				const auto setVolResp = std::static_pointer_cast<messaging::SetVolumeResponse>(message);
				printf("%s\n", setVolResp->m_result == true ? "Volume was Set" : "Volume was not set");
			}
			}
		}
	};
}

#endif // MEERKAT_SYSTEMS_CONSOLE_INPUT_H
