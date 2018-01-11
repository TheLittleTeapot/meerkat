#include "../include/Connection.h"

Connection::Connection(mg_connection& con) :
	m_connection(con)
{

}

Connection::~Connection()
{
	m_connection.user_data = nullptr;
}

void Connection::s_handler(mg_connection* nc, int ev, void * ev_data)
{
	if (!nc || !nc->user_data)
		return;

	Connection* cast = static_cast<Connection*>(nc->user_data);
	cast->eventHandler(*nc, (ConnectionEvent)ev, ev_data);
}
