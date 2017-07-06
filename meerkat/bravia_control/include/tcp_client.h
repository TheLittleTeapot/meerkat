#ifndef BRAVIA_CONTROL_TCP_CLIENT_H
#define BRAVIA_CONTROL_TCP_CLIENT_H

#include <mongoose\mongoose.h>
#include <functional>
#include <vector>

class TcpClient
{
public:
	enum class State
	{
		Disconnected,
		Connecting,
		Connected,
	};

	using ConnectedCallback = std::function<void(bool, int32_t)>;
	using DataReceivedCallback = std::function<void(std::vector<char>)>;

	TcpClient(const std::string& host, bool keep_alive):
		m_reconnect(keep_alive),
		m_host(host)
	{
		m_onConnected = [](bool success, int32_t err_code) {printf("CONNECTION RESULT: %s\n", success ? "true" : "false"); };
		m_onData = [](std::vector<char> buffer) {printf("DATA RECEIVED:\n"); for(const auto& character : buffer) printf("%c 0x%hhX\n", character, character); };
		mg_mgr_init(&m_mgr, this);

		if (m_reconnect)
		{
			m_connection = mg_connect(&m_mgr, host.c_str(), handler);
			m_connection->user_data = this;
		}

		char paramBuffer[24]{ '*', 'S', 'E', 'P', 'O', 'W', 'R', };
		memset(paramBuffer + 7, '#', 16);
		paramBuffer[23] = 0x0A;
		mg_send(m_connection, paramBuffer, 24);
	}

	~TcpClient()
	{
		mg_mgr_free(&m_mgr);
	}

	bool send(std::vector<char>& data)
	{
		if (!m_connection)
			return false;

		if (!data.size())
			return false;

		if (*data.rbegin() != 0xa)
			data.push_back('\n');

		mg_send(m_connection, data.data(), data.size());
	}

	void update()
	{
		mg_mgr_poll(&m_mgr, 10);
	}



	ConnectedCallback m_onConnected;
	DataReceivedCallback m_onData;

private:
	// Mongoose
	mg_mgr m_mgr;
	mg_connection* m_connection;

	State m_state;
	void setState(State new_state) { m_state = new_state; }

	bool m_reconnect;
	const std::string m_host;

	static void handler(struct mg_connection *nc, int ev, void *ev_data)
	{
		TcpClient& tThis = *static_cast<TcpClient*>(nc->user_data);

		switch (ev)
		{
		/* Sent to each connection on each mg_mgr_poll() call */
		case MG_EV_POLL:
			{}
			break;
		/* New connection accepted. union socket_address * */
		case MG_EV_ACCEPT: 
			{}
			break; 
		/* connect() succeeded or failed. int *  */
		case MG_EV_CONNECT: 
		{
			int success = *static_cast<int*>(ev_data);

			if (tThis.m_onConnected)
				tThis.m_onConnected(success == 0, success);
		}
		break; 
		/* Data has benn received. int *num_bytes */
		case MG_EV_RECV: {
			int length = *static_cast<int*>(ev_data);
			printf("%.*s", length, nc->recv_mbuf);

			if (tThis.m_onData)
			{
				std::vector<char> data{ nc->recv_mbuf.buf, nc->recv_mbuf.buf + length };
				tThis.m_onData(data);
			}

			mbuf_remove(&nc->recv_mbuf, length);
		}
		break; 
		/* Data has been written to a socket. int *num_bytes */
		case MG_EV_SEND:
		{
			printf("SENT %d bytes\n", *static_cast<int*>(ev_data));
		}
		break;  
		/* Connection is closed. NULL */
		case MG_EV_CLOSE: 
		{
			if (tThis.m_reconnect)
			{
				tThis.m_connection = mg_connect(&tThis.m_mgr, tThis.m_host.c_str(), handler);
				tThis.m_connection->user_data = &tThis;
			}
		}
		break; 
		/* now >= conn->ev_timer_time. double * */
		case MG_EV_TIMER: 
		{
		
		}
		break;
		}
	}
};

#endif // BRAVIA_CONTROL_TCP_CLIENT_H
