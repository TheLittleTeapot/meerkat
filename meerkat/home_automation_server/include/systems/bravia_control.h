#pragma once
#ifndef MEERKAT_SYSTEMS_BRAVIA_CONTROL_H
#define MEERKAT_SYSTEMS_BRAVIA_CONTROL_H

#include <systems/system.h>

#include <iostream>
#include <string>

namespace systems
{
	class BraviaControl : public System
	{
	public:
		BraviaControl() = default;
		~BraviaControl() = default;

	private:
		void threadUpdate() override
		{
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

				// Better set the damn volume!

				// Callback should take 'this' and send a Set_Volume_Result message
				// Alternaitvely, we can do a bind to a member function here
			}
				break;
			default:
				
				break;
			}
		}
	};
}

#endif // MEERKAT_SYSTEMS_BRAVIA_CONTROL_H