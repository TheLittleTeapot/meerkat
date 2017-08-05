#pragma once
#ifndef MEERKAT_SYSTEMS_BRAVIA_CONTROL_H
#define MEERKAT_SYSTEMS_BRAVIA_CONTROL_H

#include <systems/system.h>

#include <messaging/set_volume_response.h>
#include <bravia/bravia_controller.h>

#include <iostream>
#include <string>

namespace systems
{
	class BraviaControl : public System
	{
	public:
		BraviaControl() :
			m_bravia("192.168.1.116:20060") {}
		~BraviaControl() = default;

	private:
		void threadUpdate() override
		{
			// Gotta update the manager!
			m_bravia.update();

			// get input
			
			// Parse input to message

			// Send Message
		}

		void handleMessage(messaging::SharedMessage message) override
		{
			auto t = message->getType();

			switch (t)
			{
			case messaging::Type::Set_Volume_Request:
			{
				auto setVolumeRequest = std::static_pointer_cast<messaging::SetVolumeRequest>(message);

				m_bravia.setAudioVolume(setVolumeRequest->m_volume, 
					std::bind(&BraviaControl::handleSetAudioResponse, this, std::placeholders::_1, setVolumeRequest->m_volume));
			}
				break;
			default:
				
				break;
			}
		}

		void handleSetAudioResponse(bool success, int new_volume)
		{
			m_listener->sendMessage(std::make_shared<messaging::SetVolumeResponse>(success));
		}

	private:
		bravia::Controller m_bravia;
	};
}

#endif // MEERKAT_SYSTEMS_BRAVIA_CONTROL_H