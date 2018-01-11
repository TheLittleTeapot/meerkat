#include "../include/ListeningConnection.h"

#include "../include/IncomingConnection.h"

ListeningConnection::ListeningConnection(mg_connection & con):
	Connection(con)
{
	con.user_data = this;
}

ListeningConnection::~ListeningConnection()
{
	m_connection.flags = MG_F_CLOSE_IMMEDIATELY;
	m_incomingConnections.clear();
}

bool ListeningConnection::connectionClosed(IncomingConnection * connection)
{
	if (!connection)
		return false;

	auto findRes = std::find_if(m_incomingConnections.begin(), m_incomingConnections.end(), [connection](const std::unique_ptr<IncomingConnection>& item) {return connection == item.get(); });

	if (findRes == m_incomingConnections.end())
		return false;

	m_incomingConnections.erase(findRes);
	return true;
}

void ListeningConnection::eventHandler(mg_connection & mgCon, const ConnectionEvent evt, void * data)
{
	switch (evt)
	{
	case Connection::ConnectionEvent::Poll:
		break;
	case Connection::ConnectionEvent::Accept:
	{
		printf("ACCEPT\n");

		auto inc = std::make_unique<IncomingConnection>(mgCon);
		inc->m_connection.user_data = (void*)inc.get();
		m_incomingConnections.emplace_back(std::move(inc));
	}
	break;
	/*case Connection::ConnectionEvent::Receive:
	{
	IncomingConnection* found = (IncomingConnection*)mgCon.user_data;

	int bufferLength = *(int*)data;
	std::string buff{ mgCon.recv_mbuf.buf, mgCon.recv_mbuf.buf + bufferLength };
	mbuf_remove(&mgCon.recv_mbuf, bufferLength);
	printf("RECV: %s\n", buff.c_str());

	std::string a{ "ALLLOOO" };

	found->send({a.begin(), a.end()});
	}
	break;*/
	case Connection::ConnectionEvent::Send:
	{
		printf("SENT\n");
	}
	break;
	case Connection::ConnectionEvent::Close:
	{
		auto findRes = std::find_if(m_incomingConnections.begin(), m_incomingConnections.end(), [&mgCon](const std::unique_ptr<IncomingConnection>& item) {return mgCon.sock == item->m_connection.sock; });

		if (findRes != m_incomingConnections.end())
			m_incomingConnections.erase(findRes);

		printf("CLOSED\n");
	}
	break;
	case Connection::ConnectionEvent::Timer:
		break;
	default:
		break;

	}
}
