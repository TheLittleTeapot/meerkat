#pragma once
#include "../include/Connection.h"

#include <vector>
#include <memory>
#include <algorithm>

class IncomingConnection;

class ListeningConnection : public Connection
{
public:
	ListeningConnection(mg_connection& con);
	virtual ~ListeningConnection();

	bool connectionClosed(IncomingConnection* connection);
	

private:

	std::vector<std::unique_ptr<IncomingConnection>> m_incomingConnections;

	virtual void eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data) override;
};
