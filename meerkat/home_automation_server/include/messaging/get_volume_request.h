#ifndef MEERKAT_MESSAGING_GET_VOLUME_REQUEST_H
#define MEERKAT_MESSAGING_GET_VOLUME_REQUEST_H

#include <messaging/message.h>

namespace messaging
{
	class GetVolumeRequest : public Message
	{
	public:
		GetVolumeRequest() :
			Message(Type::Get_Volume_Request)
		{}

	};
}

#endif // MEERKAT_MESSAGING_GET_VOLUME_REQUEST_H
