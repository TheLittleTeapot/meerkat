#pragma once

#include "../include/Connection.h"
#include "../include/ListeningConnection.h"

#include <string>

class IncomingConnection : public Connection
{
public:
	IncomingConnection(mg_connection& mgConn) :
		Connection(mgConn)
	{
		mgConn.user_data = this;
	}

	void eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data) override
	{
		switch (evt)
		{
		case Connection::ConnectionEvent::Poll:
			break;
		case Connection::ConnectionEvent::Receive:
		{
			int bufferLength = *(int*)data;
			std::string buff{ mgCon.recv_mbuf.buf, mgCon.recv_mbuf.buf + bufferLength };
			mbuf_remove(&mgCon.recv_mbuf, bufferLength);
			printf("RECV: %s", buff.c_str());

			if (buff == "PING\n")
				send("PONG\n");

			if (buff == "CLOSE")
				close();
		}
		break;
		case Connection::ConnectionEvent::Send:
			break;
		case Connection::ConnectionEvent::Close:
		{
			// Should be impossible but let's cover ourselves
			if (!m_connection.listener)
				break;

			ListeningConnection* listeningConnection = static_cast<ListeningConnection*>(m_connection.listener->user_data);
			listeningConnection->connectionClosed(this);
		}
		break;
		case Connection::ConnectionEvent::Timer:
			break;
		default:
			break;
		}
	}

	void send(const std::string& buffer)
	{
		mg_send(&m_connection, buffer.data(), buffer.size());
	}

	void close()
	{
		m_connection.flags |= MG_F_CLOSE_IMMEDIATELY;
	}

private:
};