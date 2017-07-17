#ifndef MEERKAT_MESSAGE_LISTENER_H
#define MEERKAT_MESSAGE_LISTENER_H

#include <messaging/message_dispatcher.h>
#include <messaging/message.h>

#include <mutex>
#include <memory>
#include <vector>
#include <queue>

namespace messaging
{
	
	class MessageListener
	{
	public:

		MessageListener(std::shared_ptr<MessageDispatcher> dispatcher):
			m_dispatcher(dispatcher)
		{

		}

		void addMessage(SharedMessage message)
		{
			std::lock_guard<std::mutex> lk(m_vectorMutex);
			m_processQueue.push(std::move(message));
		}

		bool sendMessage(SharedMessage message)
		{
			auto shrDispatcher = m_dispatcher.lock();

			if (!shrDispatcher)
				return false;

			shrDispatcher->dispatch(std::move(message));
		}

		SharedMessage popMessage()
		{
			std::lock_guard<std::mutex> lk(m_vectorMutex);

			if (m_processQueue.empty())
				return{};

			auto uniqueMessage = std::move(m_processQueue.front());
			m_processQueue.pop();
			return std::move(uniqueMessage);
		}

	private:
		std::weak_ptr<MessageDispatcher> m_dispatcher;

		std::mutex m_vectorMutex;
		std::queue<SharedMessage> m_processQueue;
	};
}

#endif // MEERKAT_MESSAGE_LISTENER_H
