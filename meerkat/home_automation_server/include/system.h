#ifndef MEERKAT_SYSTEMS_SYSTEM_H
#define MEERKAT_SYSTEMS_SYSTEM_H

#include <messaging\message_listener.h>

#include <thread>
#include <atomic>

class System : public messaging::MessageListener
{
public:
	System():
		m_dead(false)
	{
		
	}

	virtual void init()
	{
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
	virtual void handleMessage(messaging::UniqueMessage) = 0;


private:
	std::thread m_thread;
	std::atomic_bool m_dead;

	void threadFunction()
	{
		for (;;)
		{
			if (m_dead)
				break;

			auto message = std::move(popMessage());
			if (message)
				handleMessage(std::move(message));

			threadUpdate();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
};

#endif // MEERKAT_SYSTEMS_SYSTEM_H
