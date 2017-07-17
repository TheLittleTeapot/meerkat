#ifndef MEERKAT_MESSAGING_MESSAGE_DISPATCHER_H
#define MEERKAT_MESSAGING_MESSAGE_DISPATCHER_H

#include "messaging/message_types.h"
#include "messaging/message.h"

#include <map>
#include <memory>

namespace messaging
{
	class MessageListener;

	class MessageDispatcher : public std::enable_shared_from_this<MessageDispatcher>
	{
	public:
		MessageDispatcher() = default;

		bool dispatch(SharedMessage message);

		std::unique_ptr<MessageListener> createListener(std::initializer_list<Type> types);

		bool removeListener(MessageListener* listener)
		{

		}

	private:
		//std::map<Type, MessageListener*> m_listeners;

		std::multimap<Type, MessageListener*> m_listeners;
	};
}

#endif // MEERKAT_MESSAGING_MESSAGE_DISPATCHER_H

