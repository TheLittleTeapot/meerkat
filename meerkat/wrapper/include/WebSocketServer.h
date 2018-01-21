#pragma once

#include "../include/Connection.h"

#include <vector>
#include <functional>
#include <string>

class WebSocketServer : public Connection
{
public:
	std::function<std::string(mg_connection* connection, const std::string&)> onRecv{};

	WebSocketServer(mg_connection& con);
	virtual ~WebSocketServer();

	//bool connectionClosed(IncomingConnection* connection);
	bool broadcast(const std::string&);

private:
	//std::vector<std::unique_ptr<IncomingConnection>> m_incomingConnections;

	virtual void eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data) override;
};