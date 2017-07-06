#ifndef BRAVIA_CONTROL_TCP_CLIENT_H
#define BRAVIA_CONTROL_TCP_CLIENT_H

#include <mongoose\mongoose.h>
#include <functional>

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

	TcpClient(const std::string& host)
	{
		m_onConnected = [](bool success, int32_t err_code) {printf("CONNECTION RESULT: %d", success); };
		mg_mgr_init(&m_mgr, this);

		m_connection = mg_connect(&m_mgr, host.c_str(), handler);
		m_connection->user_data = this;

		char paramBuffer[24]{ '*', 'S', 'E', 'P', 'O', 'W', 'R', };
		memset(paramBuffer + 7, '#', 16);
		paramBuffer[23] = 0x0A;
		mg_send(m_connection, paramBuffer, 24);
	}

	~TcpClient()
	{
		mg_mgr_free(&m_mgr);
	}

	void update()
	{
		mg_mgr_poll(&m_mgr, 10);
	}

	ConnectedCallback m_onConnected;

private:
	// Mongoose
	mg_mgr m_mgr;
	mg_connection* m_connection;

	State m_state;
	void setState(State new_state) { m_state = new_state; }

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
		case MG_EV_RECV: {
			int length = *static_cast<int*>(ev_data);
			printf("%.*s", length, nc->recv_mbuf);
			mbuf_remove(&nc->recv_mbuf, length);
		}break;    /* Data has benn received. int *num_bytes */
		case MG_EV_SEND:
		{
			printf("SENT!\n");
		}break;  /* Data has been written to a socket. int *num_bytes */
		case MG_EV_CLOSE: {}break; /* Connection is closed. NULL */
		case MG_EV_TIMER: {}break;  /* now >= conn->ev_timer_time. double * */

		}
	}
};

#endif // BRAVIA_CONTROL_TCP_CLIENT_H
