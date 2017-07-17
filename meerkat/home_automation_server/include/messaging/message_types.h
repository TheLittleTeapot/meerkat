#ifndef MEERKAT_MESSAGING_MESSAGE_TYPES_H
#define MEERKAT_MESSAGING_MESSAGE_TYPES_H

namespace messaging
{
	enum class Type
	{
		Unknown,
		Ping,
		Pong,

		// TV Commands
		Set_Volume_Request,
		Set_Volume_Response,
	};
}
#endif // !MEERKAT_MESSAGING_MESSAGE_TYPES_H
