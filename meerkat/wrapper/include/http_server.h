#pragma once

#include "../include/Connection.h"

class HttpServer : public Connection
{
public:
	HttpServer(mg_connection& con);
	virtual ~HttpServer();

	HttpServer(const HttpServer&) = delete;
	HttpServer(HttpServer&&) = delete;
	HttpServer& operator=(const HttpServer&) = delete;
	HttpServer& operator=(HttpServer&&) = delete;

private:
	virtual void eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data) override;
};