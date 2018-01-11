#pragma once
#include "../../middleware/mongoose/mongoose.h"

class Connection
{
public:
	Connection(mg_connection& con);
	virtual ~Connection();

	Connection(const Connection&) = delete;
	Connection(Connection&&) = delete;
	Connection& operator=(const Connection&) = delete;
	Connection& operator=(Connection&&) = delete;

	static void s_handler(struct mg_connection *nc, int ev, void *ev_data);
	
protected:
	enum class ConnectionEvent
	{
		Poll,	 /* Sent to each connection on each mg_mgr_poll() call */
		Accept,	 /* New connection accepted. union socket_address * */
		Connect, /* connect() succeeded or failed. int *  */
		Receive, /* Data has benn received. int *num_bytes */
		Send,	 /* Data has been written to a socket. int *num_bytes */
		Close,	 /* Connection is closed. NULL */
		Timer,	 /* now >= conn->ev_timer_time. double * */

		Http_Request = 100, /* struct http_message * */
		Http_Reply = 101,   /* struct http_message * */
		Http_Chunk = 102,   /* struct http_message * */
		Ssi_Call = 105,     /* char * */
		Ssi_Call_Ctx = 106, /* struct mg_ssi_call_ctx * */


		Websocket_Handshake_Request = 111, /* struct http_message * */
		Websocket_Handshake_DONE = 112,    /* NULL */
		Websocket_Frame = 113,             /* struct websocket_message * */
		Websocket_Control_Frame = 114,     /* struct websocket_message * */
	};

	virtual void eventHandler(mg_connection&, const ConnectionEvent, void* data) = 0;
public:
	mg_connection& m_connection;
};