
#include <mongoose\mongoose.h>
#include <string>

class BraviaControl
{
public:
	BraviaControl(const std::string& host, int port):
		m_host(host),
		m_port(port)
	{

	}



	bool getVolume(int& out_volume)
	{
		struct mg_mgr mgr;

		mg_mgr_init(&mgr, NULL);

		auto connection = mg_connect(&mgr, m_host.c_str(), ev_get_volume_handler);
		connection->user_data = (void*)this;

		m_commandInProgress = true;
		while(m_commandInProgress)
			mg_mgr_poll(&mgr, 100);
		



		return true;
	}



private:
	const std::string m_host;
	const int m_port;

	bool m_commandInProgress;

	std::string buff;

	// Stupid static methods
	static void ev_get_volume_handler(struct mg_connection *nc, int ev, void *ev_data)
	{
		BraviaControl& tThis = *static_cast<BraviaControl*>(nc->user_data);
		
		switch (ev)
		{
		case MG_EV_POLL :
		{}
		break;/* Sent to each connection on each mg_mgr_poll() call */
		case MG_EV_ACCEPT: {}break;   /* New connection accepted. union socket_address * */
		case MG_EV_CONNECT: {
			bool success = !*static_cast<int*>(ev_data);

			if (success)
			{
				tThis.buff = "hello";
				mg_send(nc, tThis.buff.c_str(), tThis.buff.size());
			}

		}break; /* connect() succeeded or failed. int *  */
		case MG_EV_RECV: {
			int length = *static_cast<int*>(ev_data);
			printf("%.*s", length, nc->recv_mbuf);
			mbuf_remove(&nc->recv_mbuf, length);

			mg_send(nc, tThis.buff.c_str(), tThis.buff.size());
		}break;    /* Data has benn received. int *num_bytes */
		case MG_EV_SEND:
		{
			printf("SENT!\n");
		}break;  /* Data has been written to a socket. int *num_bytes */
		case MG_EV_CLOSE: {}break; /* Connection is closed. NULL */
		case MG_EV_TIMER: {}break;  /* now >= conn->ev_timer_time. double * */

		}
		
		

	}
};

int main(void)
{
	BraviaControl bc("127.0.0.1:1234", 1234);

	int volume = 0;
	bc.getVolume(volume);

}