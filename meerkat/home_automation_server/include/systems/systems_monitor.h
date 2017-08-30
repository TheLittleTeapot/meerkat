#ifndef MEERKAT_SYSTEMS_SYSTEMS_MONITOR_H
#define MEERKAT_SYSTEMS_SYSTEMS_MONITOR_H

#include <systems/system.h>

#include <iostream>
#include <string>

#include <messaging/set_volume_request.h>
#include <messaging/set_volume_response.h>

namespace systems
{
	class SystemsMonitor : public System
	{
	public:
		SystemsMonitor() = default;
		~SystemsMonitor() = default;

	private:
		void threadUpdate() override
		{
		}

		void handleMessage(messaging::SharedMessage message) override
		{
			auto t = message->getType();

			switch (t)
			{
			case messaging::Type::Set_Volume_Request:
			{
				const auto setVolRequest = std::static_pointer_cast<messaging::SetVolumeRequest>(message);
				printf("Volume Request: %d\n", setVolRequest->m_volume);
			}
			break;
			case messaging::Type::Set_Volume_Response:
			{
				const auto setVolResp = std::static_pointer_cast<messaging::SetVolumeResponse>(message);
				printf("%s\n", setVolResp->m_result == true ? "Volume was set" : "Volume was not set");
			}
			break;
			}
		}
	};
}

#endif