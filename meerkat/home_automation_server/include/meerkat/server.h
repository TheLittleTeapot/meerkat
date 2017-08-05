#ifndef MEERKAT_SERVER_H
#define MEERKAT_SERVER_H

#include "mg_manager.h"

#include <mutex>
#include <thread>

namespace meerkat
{
	class Server
	{
	public:
		Server() = default;

		bool init(const int& port);

	private:
		PollCb		m_onPoll;
		AcceptCb	m_onAccept;
		ConnectCb	m_onConnect;
		ReceiveCb	m_onReceive;
		SendCb		m_onSend;
		CloseCb		m_onClose;


		// Multithreading
		std::thread m_serverThread;
		void threadFunction();
		volatile bool m_running;

		// Mongoose
		Manager m_manager;
	};
}

#endif // MEERKAT_SERVER_H
