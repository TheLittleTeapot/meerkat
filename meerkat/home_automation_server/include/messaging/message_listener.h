#ifndef MEERKAT_MESSAGE_LISTENER_H
#define MEERKAT_MESSAGE_LISTENER_H

#include <messaging\message.h>

#include <mutex>
#include <memory>
#include <vector>
#include <queue>

namespace messaging
{
	using UniqueMessage = std::unique_ptr<Message>;

	class MessageListener
	{
	public:
		void addMessage(UniqueMessage message)
		{
			std::lock_guard<std::mutex> lk(m_vectorMutex);
			m_processQueue.push(std::move(message));
		}
	protected:
		UniqueMessage popMessage()
		{
			std::lock_guard<std::mutex> lk(m_vectorMutex);

			if (m_processQueue.empty())
				return{};

			auto uniqueMessage = std::move(m_processQueue.front());
			m_processQueue.pop();
			return std::move(uniqueMessage);
		}

	private:
		std::mutex m_vectorMutex;
		std::queue<UniqueMessage> m_processQueue;
	};
}

#endif // MEERKAT_MESSAGE_LISTENER_H
