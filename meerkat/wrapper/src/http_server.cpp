#include "../include/http_server.h"

HttpServer::HttpServer(mg_connection & con):
	Connection(con)
{
	mg_set_protocol_http_websocket(&con);
}

HttpServer::~HttpServer()
{
}

void HttpServer::eventHandler(mg_connection & mgCon, const ConnectionEvent evt, void * data)
{
}
