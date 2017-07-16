#ifndef MEERKAT_MESSAGING_MESSAGE_DISPATCHER_H
#define MEERKAT_MESSAGING_MESSAGE_DISPATCHER_H

#include "messaging/message_types.h"
#include "messaging/message.h"

#include <map>

namespace messaging
{
	class MessageListener;
	class MessageDispatcher
	{
	public:
		MessageDispatcher() = default;

		bool dispatch(UniqueMessage message);

		bool addListener(MessageListener* listener, std::initializer_list<Type> types)
		{
			for (const auto& type : types)
			{
				m_listeners.emplace(type, listener);
			}
			return true;
		}

		bool removeListener(MessageListener* listener)
		{

		}

	private:
		std::map<Type, MessageListener*> m_listeners;
	};
}

#endif // MEERKAT_MESSAGING_MESSAGE_DISPATCHER_H

