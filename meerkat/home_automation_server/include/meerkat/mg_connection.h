#ifndef MEERKAT_CONNECTION_H
#define MEERKAT_CONNECTION_H

#include "buffer.h"

//#include <mongoose\mongoose.h>

//#include "../middleware/mongoose/mongoose.h"

#include "../../middleware/mongoose/mongoose.h"

#include <memory>
#include <functional>

namespace meerkat
{
	class Connection;

	using PollCb = std::function<void()>;
	using AcceptCb = std::function<void(const socket_address&)>;
	using ConnectCb = std::function<void(int)>;
	using ReceiveCb = std::function<void(Connection&, const Buffer&)>;
	using SendCb = std::function<void(int)>;
	using CloseCb = std::function<void()>;

	class Connection
	{
	public:
		enum class State
		{
			Disconnected,
			Connecting,
			Connected
		};
		State m_state;

		Connection(mg_connection& conn_ptr);

		void send(const Buffer& buffer);
		void send(const std::string& str_buffer);
		void close();

		PollCb		m_onPoll;
		AcceptCb	m_onAccept;
		ConnectCb	m_onConnect;
		ReceiveCb	m_onReceive;
		SendCb		m_onSend;
		CloseCb		m_onClose;

	//private:
		mg_connection& m_connection;
	};

	using WeakConnection = std::weak_ptr<Connection>;
}

#endif // MEERKAT_CONNECTION_H
