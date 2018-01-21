#include "../include/HttpClient.h"
#include <iostream>

HttpClient::HttpClient(mg_connection& con):
	Connection(con)
{
	con.user_data = this;
	mg_set_protocol_http_websocket(&con);
}

HttpClient::~HttpClient()
{
}

bool HttpClient::send(mg_connection& con, Method method, const std::string & host, const std::string path, const std::string & postData)
{
	if (method == Method::Put)
	{
		std::stringstream ss;

		ss << "PUT " << path << " HTTP/1.1\r\n";
		ss << "Host: " << host << "\r\n";
		ss << "Content-Type: application/json" << "\r\n";
		ss << "Content-Length:" << postData.size() << "\r\n";
		ss << "\r\n";

		ss << postData;
		std::string requestString = ss.str();

		mg_send(&con, requestString.c_str(), requestString.size());
		return true;
	}

	return false;
}

void HttpClient::eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data)
{
	switch (evt)
	{
	case Connection::ConnectionEvent::Poll:
		break;
	case Connection::ConnectionEvent::Accept:
		break;
	case Connection::ConnectionEvent::Connect:
	{
		bool succeeded = *(static_cast<int*>(data)) == 0;
		std::cout << std::boolalpha << succeeded << '\n';

		if (succeeded)
		{
			//send(mgCon, Method::Put, "192.168.1.27", "/api/VPYfCwtxH3cFTphV3oIEt5Gz7fQYI6p8tNucFZoq/lights/4/state", "{\"on\":false}");
			send(mgCon, Method::Put, "192.168.1.27", "/api/VPYfCwtxH3cFTphV3oIEt5Gz7fQYI6p8tNucFZoq/lights/4/state", "{\"on\":true}");

		}
	}
		break;
	case Connection::ConnectionEvent::Receive:
		break;
	case Connection::ConnectionEvent::Send:
		break;
	case Connection::ConnectionEvent::Close:
	{
		printf("CONNECTION HAS BEEN CLOSED\n");
	}
		break;
	case Connection::ConnectionEvent::Timer:
		break;
	case Connection::ConnectionEvent::Http_Request:
	{}
		break;
	case Connection::ConnectionEvent::Http_Reply:
	{
		http_message& reply = *static_cast<http_message*>(data);

		printf("%d", reply.resp_code);
	
	}
		break;
	case Connection::ConnectionEvent::Http_Chunk:
		break;
	case Connection::ConnectionEvent::Ssi_Call:
		break;
	case Connection::ConnectionEvent::Ssi_Call_Ctx:
		break;
	case Connection::ConnectionEvent::Websocket_Handshake_Request:
		break;
	case Connection::ConnectionEvent::Websocket_Handshake_DONE:
		break;
	case Connection::ConnectionEvent::Websocket_Frame:
		break;
	case Connection::ConnectionEvent::Websocket_Control_Frame:
		break;
	default:
		break;
	}
}
