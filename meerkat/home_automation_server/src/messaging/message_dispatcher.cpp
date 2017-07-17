#include <messaging/message_dispatcher.h>
#include <messaging/message_listener.h>

namespace messaging
{
	bool MessageDispatcher::dispatch(SharedMessage message)
	{
		auto& listenerRange = m_listeners.equal_range(message->getType());

		if (listenerRange.first == m_listeners.end() &&
			listenerRange.second == m_listeners.end())
			return false;


		for (auto listener = listenerRange.first; listener != listenerRange.second; ++listener)
		{
			listener->second->addMessage(std::move(message));
		}
		return true;
	}

	std::unique_ptr<MessageListener> MessageDispatcher::createListener(std::initializer_list<Type> types)
	{
		auto newListener = std::make_unique<MessageListener>(shared_from_this());
		for (const auto& type : types)
		{
			m_listeners.emplace(type, newListener.get());
		}
		return std::move(newListener);
	}
}