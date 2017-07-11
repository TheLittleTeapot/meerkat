#include "..\include\Server.h"

#include <string>

namespace meerkat
{

	bool Server::init(const int& port)
	{
		auto wkPtr = m_manager.bind(std::to_string(port));
		if (auto serverPtr = wkPtr.lock())
		{
			serverPtr->m_onAccept = [](const socket_address&) {};
		}

		m_serverThread = std::thread([this]() { threadFunction(); });

		return true;
	}

	void Server::threadFunction()
	{
		// initialisation
		
		for (;;)
		{
			if (!m_running)
				break;

			m_manager.poll(std::chrono::milliseconds{ 100 });

		}

		// do server shutdown
	}

}