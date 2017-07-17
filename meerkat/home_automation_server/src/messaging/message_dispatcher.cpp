#include <messaging/message_dispatcher.h>
#include <messaging/message_listener.h>

namespace messaging
{
	bool MessageDispatcher::dispatch(UniqueMessage message)
	{
		auto& listener = m_listeners.find(message->getType());

		if (listener == m_listeners.end())
			return false;

		listener->second->addMessage(std::move(message));
	}
}