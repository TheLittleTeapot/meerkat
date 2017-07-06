#ifndef BRAVIA_CONTROL_MG_MANAGER_H
#define BRAVIA_CONTROL_MG_MANAGER_H

#include <mongoose\mongoose.h>
#include <string>
#include <memory>
#include <chrono>

using Buffer = std::vector<char>;

class Connection
{
public:
	enum class State
	{
		Disconnected,
		Connecting,
		Connected
	};
	State m_state;

	Connection(mg_connection& conn_ptr):
		m_connection(conn_ptr)
	{
	}

	void send(Buffer& buffer)
	{
		if (m_state != State::Connected)
			return;

		mg_send(&m_connection, buffer.data(), buffer.size());
	}

	void send(const std::string& str_buffer)
	{
		Buffer b{ str_buffer.begin(), str_buffer.end() };
		send(b);
	}


	mg_connection& m_connection;
};

using unique_connection = std::unique_ptr<Connection>;


namespace Mongoose
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
			m_connections.clear();
			mg_mgr_free(&m_mgMgr);
		}

		unique_connection bind(const std::string& host)
		{
			const auto connPtr = mg_bind(&m_mgMgr, host.c_str(), s_handler);

			if (!connPtr)
				return nullptr;

			auto connection = *connPtr;
			connection.user_data = this;

			return std::make_unique<Connection>(connection);
		}

		unique_connection connect(const std::string& host)
		{
			auto connPtr = mg_connect(&m_mgMgr, host.c_str(), s_handler);

			if (!connPtr)
				return nullptr;

			auto& connection = *connPtr;
			connection.user_data = this;

			return std::make_unique<Connection>(*connPtr);
		}

		std::weak_ptr<Connection> connectWeak(const std::string& host)
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

		void internalHandler(unique_connection connection, const EventType& e, void* data)
		{
			if (e == EventType::Connect)
			{
				bool success = !*(int*)data;

				Connection* ptr = (Connection*)connection->m_connection.user_data;
				ptr->m_state = success ? Connection::State::Connected : Connection::State::Disconnected;
			}
		}

		static void s_handler(struct mg_connection *nc, int ev, void *ev_data)
		{
			unique_connection uniqueCon = nullptr;

			if (!nc || !nc->user_data)
				return;
			
			uniqueCon = std::make_unique<Connection>(*nc);

			Mongoose::Manager& manager = *static_cast<Manager*>(nc->mgr->user_data);
			manager.internalHandler(std::move(uniqueCon), (EventType)ev, ev_data);
		}
	};
}

#endif // BRAVIA_CONTROL_MG_MANAGER_H
