#ifndef MEERKAT_MANAGER_H
#define MEERKAT_MANAGER_H

#include "buffer.h"
#include "mg_connection.h"

#include "../../middleware/mongoose/mongoose.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>

namespace meerkat
{
	enum class EventType
	{
		Poll,
		Accept,
		Connect,
		Recv,
		Send,
		Close,
		Timer,
	};


	class Manager
	{
	public:

		Manager()
		{
			mg_mgr_init(&m_mgMgr, this);
		}

		~Manager()
		{
			mg_mgr_free(&m_mgMgr);
			m_connections.clear();
		}

		WeakConnection bind(const std::string& host)
		{
			const auto connPtr = mg_bind(&m_mgMgr, host.c_str(), s_handler);

			if (!connPtr)
				return {};

			auto sharedConn = std::make_shared<Connection>(*connPtr);
			m_connections.push_back(sharedConn);

			(*connPtr).user_data = sharedConn.get();

			return sharedConn;
		}

		WeakConnection connect(const std::string& host)
		{
			auto connPtr = mg_connect(&m_mgMgr, host.c_str(), s_handler);

			if (!connPtr)
				return std::weak_ptr<Connection>();

			auto& connection = *connPtr;
			connection.user_data = this;

			auto sharedConn = std::make_shared<Connection>(*connPtr);
			m_connections.push_back(sharedConn);

			connection.user_data = sharedConn.get();

			std::weak_ptr<Connection> weakPtr = sharedConn;

			return sharedConn;
		}

		void poll(std::chrono::milliseconds& sleep_millis)
		{
			mg_mgr_poll(&m_mgMgr, sleep_millis.count());
		}

	private:
		mg_mgr m_mgMgr;

		std::vector<std::shared_ptr<Connection>> m_connections;

		void internalHandler(mg_connection& connection, const EventType& e, void* data)
		{
			Connection& conn = *(Connection*)(connection.user_data);
			switch (e)
			{
			case meerkat::EventType::Poll:
			{
				conn.m_onPoll();
			}
				break;
			case meerkat::EventType::Accept:
			{
				socket_address& sockAddr = *(socket_address*)data;
				conn.m_onAccept(sockAddr);
			}
				break;
			case meerkat::EventType::Connect:
			{
				int result = *(int*)data;
				conn.m_onConnect(result);
			}
				break;
			case meerkat::EventType::Recv:
			{
				Connection incomingConnection{ connection };

				int bufferLength = *(int*)data;
				meerkat::Buffer buff{ connection.recv_mbuf.buf, connection.recv_mbuf.buf + bufferLength };
				conn.m_onReceive(incomingConnection, buff);
				mbuf_remove(&connection.recv_mbuf, bufferLength);
			}
				break;
			case meerkat::EventType::Send:
			{
				int bytesSent = *(int*)data;
				conn.m_onSend(bytesSent);
			}
				break;
			case meerkat::EventType::Close:

				conn.m_onClose();
				m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(), [&conn](const std::shared_ptr<Connection>& connection) {return &conn.m_connection == &connection->m_connection;}), m_connections.end());

				break;
			case meerkat::EventType::Timer:
				
				break;
			default:
				break;
			}
			

		}

		static void s_handler(struct mg_connection *nc, int ev, void *ev_data)
		{
			if (!nc || !nc->user_data)
				return;

			meerkat::Manager& manager = *static_cast<meerkat::Manager*>(nc->mgr->user_data);
			manager.internalHandler(*nc, (EventType)ev, ev_data);
		}
	};
}


#endif // MEERKAT_MANAGER_H
