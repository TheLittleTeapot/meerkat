#ifndef MEERKAT_MESSAGE_H
#define MEERKAT_MESSAGE_H

#include "messaging\message_types.h"

#include <memory>
#include <vector>

namespace messaging
{

	class Message
	{
	public:
		Message(Type type): m_type(type){}

		Type getType() const
		{
			return m_type;
		}

		int m_transactionId;
	private:
		Type m_type;
	};

	//using UniqueMessage = std::unique_ptr<Message>;
	using SharedMessage = std::shared_ptr<Message>;
}

#endif // MEERKAT_MESSAGE_H
