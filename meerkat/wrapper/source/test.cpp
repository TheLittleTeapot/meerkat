#include "../include/Test.h"

#include <mongoose\mongoose.h>

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct mbuf *io = &nc->recv_mbuf;

	switch (ev) {
	case MG_EV_RECV:
		// This event handler implements simple TCP echo server
		mg_send(nc, io->buf, io->len);  // Echo received data back
		printf("%.*s", io->len, io->buf);
		mbuf_remove(io, io->len);      // Discard data from recv buffer
		break;
	default:
		break;
	}
}

int testFunc(int a)
{

	struct mg_mgr mgr;

	mg_mgr_init(&mgr, NULL);  // Initialize event manager object

							  // Note that many connections can be added to a single event manager
							  // Connections can be created at any point, e.g. in event handler function
	mg_bind(&mgr, "1234", ev_handler);  // Create listening connection and add it to the event manager

	for (;;) {  // Start infinite event loop
		mg_mgr_poll(&mgr, 1000);
	}

	mg_mgr_free(&mgr);
	return 0;

	return 0;
}
