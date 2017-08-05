#include "meerkat/mg_connection.h"

namespace meerkat
{
	Connection::Connection(mg_connection& conn_ptr):
	m_connection(conn_ptr)
	{
		m_onPoll = []() {};
		m_onAccept = [](const socket_address&) {};
		m_onConnect = [](int) {};
		m_onReceive = [](Connection& connection, const Buffer&) {};
		m_onSend = [](int) {};
		m_onClose = []() {};
	}

	void Connection::send(const Buffer& buffer)
	{
		mg_send(&m_connection, buffer.data(), buffer.size());
	}

	void Connection::send(const std::string& str)
	{
		Buffer b{ str.begin(), str.end() };
		send(b);
	}

	void Connection::close()
	{
		m_connection.flags |= MG_F_CLOSE_IMMEDIATELY;
	}

}