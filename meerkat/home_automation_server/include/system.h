#ifndef MEERKAT_SYSTEMS_SYSTEM_H
#define MEERKAT_SYSTEMS_SYSTEM_H

#include <messaging\message_listener.h>

#include <thread>
#include <atomic>

class System
{
public:
	System():
		m_dead(false)
	{
		
	}

	virtual void init(std::unique_ptr<messaging::MessageListener> listener_ptr)
	{
		m_listener = std::move(listener_ptr);
		m_thread = std::thread(&System::threadFunction, this);
	}

	virtual void term()
	{
		m_dead = true;
		m_thread.join();
		m_thread = {};
	}

protected:

	virtual void threadUpdate() = 0;
	virtual void handleMessage(messaging::SharedMessage) = 0;
	std::unique_ptr<messaging::MessageListener> m_listener;


private:
	std::thread m_thread;
	std::atomic_bool m_dead;

	void threadFunction()
	{
		for (;;)
		{
			if (m_dead)
				break;

			if (m_listener)
			{
				auto message = m_listener->popMessage();
				if (message)
					handleMessage(message);
			}

			threadUpdate();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
};

#endif // MEERKAT_SYSTEMS_SYSTEM_H
