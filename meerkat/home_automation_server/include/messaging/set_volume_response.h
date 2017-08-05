#ifndef MEERKAT_MESSAGING_SET_VOLUME_RESPONSE_H
#define MEERKAT_MESSAGING_SET_VOLUME_RESPONSE_H

#include <messaging/message.h>

namespace messaging
{
	class SetVolumeResponse : public Message
	{
	public:
		SetVolumeResponse(bool result) :
			messaging::Message(Type::Set_Volume_Response),
			m_result(result)
		{}

		bool m_result;
	};
}

#endif // MEERKAT_MESSAGING_SET_VOLUME_RESPONSE_H
