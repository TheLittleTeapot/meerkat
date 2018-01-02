#include "../../middleware/mongoose/mongoose.h"


class Connection
{
public:
	Connection(const mg_connection& con) : m_connection(con) {};
	virtual ~Connection() = default;

	Connection(const Connection&) = delete;
	Connection(Connection&&) = delete;
	Connection& operator=(const Connection&) = delete;
	Connection& operator=(Connection&&) = delete;

	static void s_handler(struct mg_connection *nc, int ev, void *ev_data)
	{
		if (!nc || !nc->user_data)
			return;

		Connection* cast = static_cast<Connection*>(nc->user_data);
		cast->eventHandler(*nc, (ConnectionEvent)ev, ev_data);
	}
protected:
	enum class ConnectionEvent
	{
		Poll,	 /* Sent to each connection on each mg_mgr_poll() call */
		Accept,	 /* New connection accepted. union socket_address * */
		Connect, /* connect() succeeded or failed. int *  */
		Receive, /* Data has benn received. int *num_bytes */
		Send,	 /* Data has been written to a socket. int *num_bytes */
		Close,	 /* Connection is closed. NULL */
		Timer,	 /* now >= conn->ev_timer_time. double * */
	};


	virtual void eventHandler(mg_connection&, const ConnectionEvent, void* data) = 0;

private:
	mg_connection m_connection;


};

class ListeningConnection : public Connection
{
public:
	ListeningConnection(mg_connection& con):
		Connection(con)
	{
		con.user_data = this;
	}

private:
	void eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data) override
	{
		switch (evt)
		{
		case Connection::ConnectionEvent::Poll:
			//printf("POLL\n");
			break;
		case Connection::ConnectionEvent::Accept:
			socket_address sock = *(socket_address*)data;

			printf("ACCEPT\n");
			break;
		case Connection::ConnectionEvent::Receive:
			break;
		case Connection::ConnectionEvent::Send:
			break;
		case Connection::ConnectionEvent::Close:
			break;
		case Connection::ConnectionEvent::Timer:
			break;
		default:
			break;

		}
	}

};

class IncomingConnection : public Connection
{

};

class OutgoingConnection : public Connection
{

};

int main()
{
	mg_mgr mgr;
	mg_mgr_init(&mgr, nullptr);

	const auto mgCon = mg_bind(&mgr, "8080", Connection::s_handler);
	ListeningConnection listenConn{ *mgCon };

	const auto client = mg_connect(&mgr, "192.168.1.2:8080", Connection::s_handler);


	for (;;)
	{
		mg_mgr_poll(&mgr, 10);
	}

	return 0;
}