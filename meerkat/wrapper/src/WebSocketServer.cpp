#include "../include/WebSocketServer.h"

WebSocketServer::WebSocketServer(mg_connection & con):
	Connection(con)
{
	mg_set_protocol_http_websocket(&con);
	con.user_data = this;
}

WebSocketServer::~WebSocketServer()
{
	m_connection.flags = MG_F_CLOSE_IMMEDIATELY;
}

bool WebSocketServer::send(const std::string &)
{
	return false;
}

void WebSocketServer::eventHandler(mg_connection & mgCon, const ConnectionEvent evt, void * data)
{
	switch (evt)
	{
	case Connection::ConnectionEvent::Poll:
		break;
	case Connection::ConnectionEvent::Accept:
		break;
	
	case Connection::ConnectionEvent::Receive:
	{
		int bufferLength = *(int*)data;
		std::string buff{ mgCon.recv_mbuf.buf, mgCon.recv_mbuf.buf + bufferLength };
		//mbuf_remove(&mgCon.recv_mbuf, bufferLength);
		printf("RECV: %s\n", buff.c_str());
	}
		break;
	case Connection::ConnectionEvent::Send:
		break;
	case Connection::ConnectionEvent::Close:
		break;
	case Connection::ConnectionEvent::Timer:
		break;
	case Connection::ConnectionEvent::Http_Request:
		break;
	case Connection::ConnectionEvent::Http_Reply:
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
	{
		websocket_message* wsMsg = static_cast<websocket_message*>(data);
		std::string dataStr{ wsMsg->data, wsMsg->data + wsMsg->size };

		if (onRecv)
		{
			auto retString = onRecv(dataStr);
			mg_send_websocket_frame(&mgCon, WEBSOCKET_OP_TEXT, retString.c_str(), retString.size());
		}

		//std::string str{ "Hello Client!" };
		//	mg_send_websocket_frame(&mgCon, WEBSOCKET_OP_TEXT, str.c_str(), str.size());
	}
		break;
	case Connection::ConnectionEvent::Websocket_Control_Frame:
	{
//#define WEBSOCKET_OP_CONTINUE 0
//#define WEBSOCKET_OP_TEXT 1
//#define WEBSOCKET_OP_BINARY 2
//#define WEBSOCKET_OP_CLOSE 8
//#define WEBSOCKET_OP_PING 9
//#define WEBSOCKET_OP_PONG 10
//	
		websocket_message* wsMsg = static_cast<websocket_message*>(data);
		if (wsMsg->flags & WEBSOCKET_OP_PING)
			mg_printf_websocket_frame(&mgCon, WEBSOCKET_OP_PONG, "");
	}
		break;
	default:
		break;
	}
}
