#ifndef MEERKAT_MESSAGING_SET_VOLUME_REQUEST_H
#define MEERKAT_MESSAGING_SET_VOLUME_REQUEST_H

#include <messaging/message.h>

namespace messaging
{
	class SetVolumeRequest : public Message
	{
	public:
		SetVolumeRequest(int volume) :
			messaging::Message(Type::Set_Volume_Request),
			m_volume(volume)
		{}

		int m_volume;
	};
}

#endif // MEERKAT_MESSAGING_SET_VOLUME_REQUEST_H
