#ifndef MEERKAT_SYSTEMS_CONSOLE_INPUT_H
#define MEERKAT_SYSTEMS_CONSOLE_INPUT_H

#include <systems/system.h>

#include <iostream>
#include <string>

#include <messaging/set_volume_request.h>
#include <messaging/get_volume_request.h>

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
			// get input
			std::string input;
			std::getline(std::cin, input);

			if (!input.empty())
			{
				if (*input.begin() == 'G')
				{
					m_listener->sendMessage(std::make_shared<messaging::GetVolumeRequest>());
				}
				else
				{
					m_listener->sendMessage(std::make_shared<messaging::SetVolumeRequest>(std::stoi(input)));
				}
			}
				

			// Parse input to message
			
			// Send Message
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

		void handleMessage(messaging::SharedMessage message) override
		{
			auto t = message->getType();

			switch (t)
			{
			}
		}
	};
}

#endif // MEERKAT_SYSTEMS_CONSOLE_INPUT_H
