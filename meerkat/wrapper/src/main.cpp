//#include "../../middleware/mongoose/mongoose.h"

#include "../include/Connection.h"

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <algorithm>
#include <array>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <random>

#include <regex>

#include "../include/ListeningConnection.h"
#include "../include/IncomingConnection.h"
#include "../include/WebSocketServer.h"
#include "../include/http_server.h"
#include "../include/HttpClient.h"

#include "../../home_automation_server/include/utilities/autokey_cypher.h"

class OutgoingConnection : public Connection
{
public:
	using GenericCallback = std::function<void(OutgoingConnection& connection)>;
	using ReceiveCallback = std::function<void(OutgoingConnection& connection, const std::string& data)>;

	GenericCallback onConnect {};
	GenericCallback onDisconnect {};
	ReceiveCallback onReceive {};

	OutgoingConnection(mg_connection& mgConn) :
		Connection(mgConn)
	{
		mgConn.user_data = this;
	}

	void send(const std::string& buffer)
	{
		mg_send(&m_connection, buffer.data(), buffer.size());
	}

	void eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data) override
	{
		switch (evt)
		{
		case Connection::ConnectionEvent::Poll:
			break;
		case Connection::ConnectionEvent::Connect:
		{
			if (onConnect)
				onConnect(*this);
		}
		break;
		case Connection::ConnectionEvent::Receive:
		{
			int bufferLength = *(int*)data;
			std::string buff{ mgCon.recv_mbuf.buf, mgCon.recv_mbuf.buf + bufferLength };
			mbuf_remove(&mgCon.recv_mbuf, bufferLength);

			if (onReceive)
				onReceive(*this, buff);
		}
		break;
		case Connection::ConnectionEvent::Send:
			break;
		case Connection::ConnectionEvent::Close:
		{
			if (onDisconnect)
				onDisconnect(*this);
		}
		break;
		case Connection::ConnectionEvent::Timer: 
		{

		}
			break;
		default:
			break;
		}
	}

private:

};

enum class Function
{
	Ircc,
	Power,
	Volume,
	AudioMute,
	Channel,
	TripletChannel,
	InputSource,
	Input,
	PictureMute,
	PictureMuteToggle,
	PictureInPicture,
	PictureInPictureToggle,
	PictureInPicturePositionToggle,
	BroadcastAddress,
	MacAddress,
};

static std::map<Function, const char*> functionCodes
{
	{ Function::Ircc, "IRCC" },
	{ Function::Power, "POWR" },
	{ Function::Volume, "VOLU" },
	{ Function::AudioMute, "AMUT" },
	{ Function::Channel, "CHNN" },
	{ Function::TripletChannel, "TCHN" },
	{ Function::InputSource, "ISRC" },
	{ Function::Input, "INPT" },
	{ Function::PictureMute, "PMUT" },
	{ Function::PictureMuteToggle, "TPMU" },
	{ Function::PictureInPicture, "PIPI" },
	{ Function::PictureInPictureToggle, "TPIP" },
	{ Function::PictureInPicturePositionToggle, "TPPP" },
	{ Function::BroadcastAddress, "BADR" },
	{ Function::MacAddress, "MADR" }
};

static const int functionLength = 4;
/////////////

class BraviaCommand
{
public:
	enum class CommandType
	{
		Control = 0x43,
		Enquiry = 0x45,
		Answer = 0x41,
		Notify = 0x4E,
	};

	BraviaCommand(const CommandType& type, const Function& function)
	{
		m_buffer[0] = '*';
		m_buffer[1] = 'S';
		m_buffer[23] = '\n';

		setCommandType(type);
		setFunction(function);
	}

	virtual void issueCallback(std::array<char, 16> buff) = 0;

	operator const std::vector<char>() const
	{
		return{ m_buffer.begin(), m_buffer.end() };
	}

	operator const std::string() const
	{
		return{ m_buffer.begin(), m_buffer.end() };
	}

	CommandType getCommandType() const
	{
		return static_cast<CommandType>(m_buffer[2]);
	}

	Function getFunction() const
	{
		std::string functionName = { m_buffer.begin() + 3, m_buffer.begin() + 7 };
		auto findRes = std::find_if(functionCodes.begin(), functionCodes.end(), [&functionName](const std::pair<Function, std::string>& pair) {return pair.second == functionName; });

		if (findRes == functionCodes.end())
			return Function::AudioMute;

		return findRes->first;
	}

protected:
	bool m_success;

	void setParameterBuffer(std::array<char, 16> paramBuffer)
	{
		std::memcpy(&m_buffer[7], &paramBuffer[0], paramBuffer.size());
	}

private:

	void setCommandType(const CommandType type)
	{
		m_buffer[2] = static_cast<char>(type);
	}

	void setFunction(const Function& function)
	{
		std::memcpy(&m_buffer[3], functionCodes[function], functionLength);
	}

	std::array<char, 24> m_buffer;
};

class BoolCommand : public BraviaCommand
{
public:
	using BoolCallback = std::function<void(const bool&, const bool&)>;
	BoolCommand(BoolCallback cb, CommandType type, Function function, bool status) :
		BraviaCommand(type, function),
		m_cb(cb),
		m_status(status)
	{
		std::stringstream ss;
		ss << std::setw(16) << std::right << std::setfill('0') << static_cast<int>(status);
		std::string paramStr{ ss.str() };
		std::array<char, 16> paramBuffer;
		std::copy(paramStr.begin(), paramStr.end(), paramBuffer.begin());
		setParameterBuffer(paramBuffer);
	}

	void issueCallback(std::array<char, 16> buff) override
	{
		const bool error = std::all_of(buff.begin(), buff.end(), [](const char& a) {return a == 'F'; });
		m_cb(!error, m_status);
	}

protected:
	BoolCallback m_cb;
	bool m_status;
};

class SetAudioMute : public BoolCommand
{
public:
	SetAudioMute(const bool& status, BoolCallback cb):
		BoolCommand(cb, CommandType::Control, Function::AudioMute, status)
	{}
};

class SetPictureMute : public BoolCommand
{
public:
	SetPictureMute(const bool& status, BoolCallback cb) :
		BoolCommand(cb, CommandType::Control, Function::PictureMute, status)
	{}
};

class SetVolume :public BraviaCommand
{
public:
	SetVolume(std::function<void(const bool&, const int&)> cb, int volume) :
		BraviaCommand(CommandType::Control, Function::Volume),
		m_cb(cb),
		m_volume(volume)
	{
		std::stringstream ss;
		ss << std::setw(16) << std::right << std::setfill('0') << volume;
		
		std::array<char, 16> paramBuffer;
		std::string paramStr{ ss.str() };

		std::copy(std::begin(paramStr), std::end(paramStr), paramBuffer.begin());

		setParameterBuffer(paramBuffer);
	}

	void issueCallback(std::array<char, 16> buff) override
	{
		std::string str{ buff.begin(), buff.end() };
		if (std::all_of(buff.begin(), buff.end(), [](const auto& a) {return a == 'F';})) // All F's, error
		{
			m_cb(false, m_volume);
			return;
		}

		auto retVal = std::stoi(str);
		m_cb((retVal == 0), m_volume);
	}
private:
	int m_volume;

	std::function<void(const bool&, const int&)> m_cb;
};

class GetVolume : public BraviaCommand
{
public:
	GetVolume(std::function<void(const bool&, const int&)> cb) :
		BraviaCommand(CommandType::Enquiry, Function::Volume),
		m_cb(cb)
	{
		std::array<char, 16> paramBuffer;
		std::memset(paramBuffer.data(), '#', 16);
		setParameterBuffer(paramBuffer);
	}

	void issueCallback(std::array<char, 16> buff) override
	{
		std::string str{ buff.begin(), buff.end() };
		auto retVal = std::stoi(str);
		m_cb((retVal == 0), retVal);
	}

private:
	std::function<void(const bool&, const int&)> m_cb;
};

class GetPower : public BraviaCommand
{
public:
	GetPower(std::function<void(const bool&, const bool&)> cb) :
		BraviaCommand(CommandType::Enquiry, Function::Power),
		m_cb(cb)
	{
		std::array<char, 16> paramBuffer;
		std::memset(paramBuffer.data(), '#', 16);
		setParameterBuffer(paramBuffer);
	}

	void issueCallback(std::array<char, 16> buff) override
	{
		std::string str{ buff.begin(), buff.end() };
		const bool error = std::all_of(buff.begin(), buff.end(), [](const char& a) {return a == 'F'; });

		if (error)
		{
			m_cb(false, -1);
			return;
		}

		auto retVal = std::stoi(str);
		m_cb(true, retVal);
	}

private:
	std::function<void(const bool&, const int&)> m_cb;
};

class SetPower : public BraviaCommand
{
public:
	SetPower(std::function<void(const bool&, const bool&)> cb, bool newStatus) :
		BraviaCommand(CommandType::Control, Function::Power),
		m_cb(cb)
	{
		std::stringstream ss;
		ss << std::setw(16) << std::right << std::setfill('0') << static_cast<int>(newStatus);
		std::string paramStr{ ss.str() };
		std::array<char, 16> paramBuffer;
		std::copy(paramStr.begin(), paramStr.end(), paramBuffer.begin());
		setParameterBuffer(paramBuffer);
	}

	void issueCallback(std::array<char, 16> buff) override
	{
		std::string str{ buff.begin(), buff.end() };
		const bool error = std::all_of(buff.begin(), buff.end(), [](const char& a) {return a == 'F'; });

		if (error)
		{
			m_cb(false, false);
			return;
		}

		auto retVal = std::stoi(str);
		m_cb(true, retVal);
	}

private:
	std::function<void(const bool&, const int&)> m_cb;
};








bool getBufferInfo(const std::string& buffer, BraviaCommand::CommandType& type, Function& function)
{
	if (buffer.size() != 24)
		return false;

	char typeChr = buffer[2];
	if (typeChr == 'C') type = BraviaCommand::CommandType::Control;
	else if (typeChr == 'E') type = BraviaCommand::CommandType::Enquiry;
	else if (typeChr == 'A') type = BraviaCommand::CommandType::Answer;
	else if (typeChr == 'N') type = BraviaCommand::CommandType::Notify;
	else return false;

	std::string functionName = { buffer.begin() + 3, buffer.begin() + 7 };
	auto findRes = std::find_if(functionCodes.begin(), functionCodes.end(), [&functionName](const std::pair<Function, std::string>& pair) {return pair.second == functionName; });

	if (findRes == functionCodes.end())
		return false;

	function = findRes->first;

	return true;
}

class BraviaClient : public OutgoingConnection
{
public:
	BraviaClient(mg_connection& mgConn) :
		OutgoingConnection(mgConn)
	{
		mgConn.user_data = this;
		onReceive = std::bind(&BraviaClient::internalRecv, this, std::placeholders::_1, std::placeholders::_2);
	}

	using SetVolumeCb = std::function<void(BraviaClient*, const bool&, const int&)>;
	bool setVolume(const int& volume, SetVolumeCb volumeCb)
	{
		auto cb = std::bind(volumeCb, this, std::placeholders::_1, std::placeholders::_2);
		SetVolume setV{ cb, volume };

		std::unique_ptr<BraviaCommand> uniqueSetV{ std::make_unique<SetVolume>(cb,volume) };

		std::vector<char> test{ setV };

		std::string strBuff{ test.begin(), test.end() };
		
		std::pair<BraviaCommand::CommandType, Function> key{ BraviaCommand::CommandType::Answer, setV.getFunction() };
		m_pendingCallbacks.emplace_back(key, std::move(uniqueSetV));

		send(strBuff);
		return true;
	}

	using GetVolumeCb = std::function<void(BraviaClient*, const bool&, const int&)>;
	bool getVolume(GetVolumeCb cb)
	{
		auto getVolumeCb = std::bind(cb, this, std::placeholders::_1, std::placeholders::_2);

		std::unique_ptr<BraviaCommand> uniqueGetV{ std::make_unique<GetVolume>(getVolumeCb) };
		send((*uniqueGetV));

		std::pair<BraviaCommand::CommandType, Function> key{ BraviaCommand::CommandType::Answer, uniqueGetV->getFunction() };
		m_pendingCallbacks.emplace_back(key, std::move(uniqueGetV));

		return true;
	}

	using GetPowerCb = std::function<void(BraviaClient*, const bool&, const bool&)>;
	bool getPowerStatus(GetPowerCb cb)
	{
		auto getPowerCb = std::bind(cb, this, std::placeholders::_1, std::placeholders::_2);

		std::unique_ptr<BraviaCommand> uniqueGetPower{ std::make_unique<GetPower>(getPowerCb) };
		send((*uniqueGetPower));

		std::pair<BraviaCommand::CommandType, Function> key{ BraviaCommand::CommandType::Answer, uniqueGetPower->getFunction() };
		m_pendingCallbacks.emplace_back(key, std::move(uniqueGetPower));

		return true;
	}

	using SetPowerCb = std::function<void(BraviaClient*, const bool&, const bool&)>;
	bool setPowerStatus(const bool& newStatus, GetPowerCb cb)
	{
		auto setPowerCb = std::bind(cb, this, std::placeholders::_1, std::placeholders::_2);

		std::unique_ptr<BraviaCommand> uniqueSetPower{ std::make_unique<SetPower>(setPowerCb, newStatus) };
		send((*uniqueSetPower));

		std::pair<BraviaCommand::CommandType, Function> key{ BraviaCommand::CommandType::Answer, uniqueSetPower->getFunction() };
		m_pendingCallbacks.emplace_back(key, std::move(uniqueSetPower));

		return true;
	}

	using SetAudioMuteCb = std::function<void(BraviaClient*, const bool&, const bool&)>;
	bool setAudioMute(const bool& newStatus, SetAudioMuteCb cb)
	{
		auto setAudioMuteCb = std::bind(cb, this, std::placeholders::_1, std::placeholders::_2);
		std::unique_ptr<BraviaCommand> uniqueSetAudioMute{ std::make_unique<SetAudioMute>(newStatus, setAudioMuteCb) };
		send((*uniqueSetAudioMute));

		std::pair<BraviaCommand::CommandType, Function> key{ BraviaCommand::CommandType::Answer, uniqueSetAudioMute->getFunction() };
		m_pendingCallbacks.emplace_back(key, std::move(uniqueSetAudioMute));

		return true;
	}

	using SetPictureMuteCb = std::function<void(BraviaClient*, const bool&, const bool&)>;
	bool setPictureMute(const bool& newStatus, SetPictureMuteCb cb)
	{
		auto setPictureMuteCb = std::bind(cb, this, std::placeholders::_1, std::placeholders::_2);
		std::unique_ptr<BraviaCommand> uniqueSetPictureMute{ std::make_unique<SetPictureMute>(newStatus, setPictureMuteCb) };
		send((*uniqueSetPictureMute));

		std::pair<BraviaCommand::CommandType, Function> key{ BraviaCommand::CommandType::Answer, uniqueSetPictureMute->getFunction() };
		m_pendingCallbacks.emplace_back(key, std::move(uniqueSetPictureMute));
		return true;
	}

	static std::unique_ptr<BraviaClient> create(mg_mgr& mgCon, const std::string& host)
	{
		const auto mgClient = mg_connect(&mgCon, host.c_str(), Connection::s_handler);

		if (!mgClient)
			return{};

		return std::make_unique<BraviaClient>(*mgClient);
	}

private:

	std::vector<std::pair<std::pair<BraviaCommand::CommandType, Function>, std::unique_ptr<BraviaCommand> >> m_pendingCallbacks;

	void internalRecv(OutgoingConnection& connection, const std::string& data)
	{
		BraviaCommand::CommandType type;
		Function function;
		auto ret = getBufferInfo(data, type, function);

		std::pair<BraviaCommand::CommandType, Function> val{ type, function };

		for (auto itr = m_pendingCallbacks.begin(); itr != m_pendingCallbacks.end(); )
		{
			if ((*itr).first == val)
			{
				std::string paramBuffer{ data.begin() + 7, data.begin() + 23 };
				std::array<char, 16> paramBuff{};
				std::copy(paramBuffer.begin(), paramBuffer.end(), paramBuff.begin());

				(*itr).second->issueCallback(paramBuff);

				itr = m_pendingCallbacks.erase(itr);
				continue;
			}

			itr++;
		}

		if (type == BraviaCommand::CommandType::Notify)
		{
			printf("I just want you to know that %s", data.c_str());
		}
	}
};

class Manager
{
public:
	Manager()
	{
		mg_mgr_init(&m_mgMgr, this);
	}

	virtual ~Manager()
	{
		mg_mgr_free(&m_mgMgr);
	}

	std::unique_ptr<ListeningConnection> createServer(const std::string& host)
	{
		const auto mgCon = mg_bind(&m_mgMgr, host.c_str(), Connection::s_handler);

		if (!mgCon)
			return{};

		return std::make_unique<ListeningConnection>(*mgCon);
	}

	std::unique_ptr<WebSocketServer> createWebSocketServer(const std::string& host)
	{
		const auto mgCon = mg_bind(&m_mgMgr, host.c_str(), Connection::s_handler);
		

		if (!mgCon)
			return{};

		return std::make_unique<WebSocketServer>(*mgCon);
	}

	std::unique_ptr<OutgoingConnection> createClient(const std::string& host)
	{
		const auto mgClient = mg_connect(&m_mgMgr, host.c_str(), Connection::s_handler);

		if (!mgClient)
			return{};
		
		return std::make_unique<OutgoingConnection>(*mgClient);
	}

	std::unique_ptr<BraviaClient> braviaClient(const std::string& host)
	{
		const auto mgClient = mg_connect(&m_mgMgr, host.c_str(), Connection::s_handler);

		if (!mgClient)
			return{};

		return std::make_unique<BraviaClient>(*mgClient);
	}

	std::unique_ptr<HttpClient> httpClient(const std::string& host)
	{
		const auto mgClient = mg_connect(&m_mgMgr, host.c_str(), Connection::s_handler);

		if (!mgClient)
			return{};

		return std::make_unique<HttpClient>(*mgClient);
	}

	void poll(const int& millis)
	{
		mg_mgr_poll(&m_mgMgr, millis);
	}

	operator mg_mgr&()
	{
		return m_mgMgr;
	}


private:
	mg_mgr m_mgMgr;
};

void toBuffer(unsigned int value, std::vector<char>& buffer)
{
	buffer.push_back((value >> 24) & 0xFF);
	buffer.push_back((value >> 16) & 0xFF);
	buffer.push_back((value >> 8) & 0xFF);
	buffer.push_back((value) & 0xFF);
}


class HttpRequest
{
public:

	std::string url;
	Method method;
	std::map<std::string, std::string> headers;
	std::string body;
};

const static std::map<Method, const char*> methods
{
	{ Method::Head, "HEAD" },
	{ Method::Get, "GET" },
	{ Method::Post, "POST" },
	{ Method::Put, "PUT" },
	{ Method::Patch, "PATCH" },
	{ Method::Delete, "DELETE" },
};

class HttpC
{
public:
	using Host = std::string;

	HttpC(Manager& man):
		m_manager(man)
	{
	}

	bool send(const HttpRequest& request /*, and callback*/)
	{
		//Host host{ "example.com:80" };
		std::regex url_regex(R"(^(([^:\/?#]+):)?(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)", std::regex::extended);
		std::smatch url_match_result;

		if (!std::regex_match(request.url, url_match_result, url_regex))
		{
			std::cerr << "Malformed url." << std::endl;
			return false;
		}
		
		std::string authority = { url_match_result[4].first, url_match_result[4].second };
		std::string path = { url_match_result[5].first, url_match_result[5].second };

		auto session = m_connections.find(authority);

		mg_connection* connection = nullptr;
		if (session != m_connections.end())
		{
			connection = session->second;
		}
		else
		{
			connection = mg_connect(&m_manager, authority.c_str(), s_handler);
			connection->user_data = this;
			mg_set_protocol_http_websocket(connection);
			m_connections.emplace(authority, connection);
		}

		std::stringstream ss;
		ss << methods.at(request.method) << " " << path << " HTTP/1.1\r\n";
		ss << "Host: " << authority << "\r\n";

		if (request.body.size())
		{
			ss << "Content-Length: " << request.body.size() << "\r\n";
		}

		ss << "\r\n";

		if (request.body.size())
		{
			ss << request.body;
		}

		std::string str{ ss.str() };

			
		mg_send(connection, str.c_str(), str.size());
		return true;
	}


private:
	mg_mgr& m_manager;

	bool removeConnection(mg_connection* connection)
	{
		auto findRes = std::find_if(m_connections.begin(), m_connections.end(), [connection](const std::pair<Host, mg_connection*>& pair)
		{
			return pair.second == connection;
		});

		if (findRes == m_connections.end())
			return false;

		return m_connections.erase(findRes->first);
	}

	static void s_handler(struct mg_connection *nc, int ev, void *ev_data)
	{
		if (ev == 5)
		{
			printf("CLOSED\n");
			HttpC client = *static_cast<HttpC*>(nc->user_data);
			client.removeConnection(nc);
		}

		if (ev == 101)
		{
			http_message& reply = *static_cast<http_message*>(ev_data);

			printf("%d\n", reply.resp_code);

			std::string s{ reply.body.p, reply.body.len };
			printf("%s\n", s.c_str());
		}
	}

	std::map<Host, mg_connection*> m_connections;
};


int main()
{
	

	unsigned counter = 0;

	std::string url{ "192.168.1.27/api/VPYfCwtxH3cFTphV3oIEt5Gz7fQYI6p8tNucFZoq/lights/4/state" };

	std::regex url_regex(R"(^(([^:\/?#]+):)?(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)", std::regex::extended);
	std::smatch url_match_result;

	std::cout << "Checking: " << url << std::endl;

	if (std::regex_match(url, url_match_result, url_regex)) {
		for (const auto& res : url_match_result) {
			std::cout << counter++ << ": " << res << std::endl;
		}
	}
	else {
		std::cerr << "Malformed url." << std::endl;
	}



	Manager manager;


	HttpC cll{ manager };
	HttpRequest req;

	req.method = Method::Get;

	//req.url = "http://192.168.1.27:80/api/VPYfCwtxH3cFTphV3oIEt5Gz7fQYI6p8tNucFZoq/lights/4/state";
	req.url = "http://httpbin.org:80/ip";
	//req.body = "{\"on\":false}";
	cll.send(req);
	

	// SSDP 
	/*
	const auto client = manager.createClient("udp://239.255.255.250:1900");
	client->onConnect = [](auto& connection) { printf("Connected\n"); };
	client->send("M-SEARCH * HTTP/1.1\r\n"
		"HOST: 239.255.255.250:1900\r\n"
		"MAN: \"ssdp:discover\"\r\n"
		"MX: 5\r\n"
		"ST: ssdp:all\r\n");

	client->onReceive = [](auto& con, const auto& data) {printf("%s", data.c_str()); };
	*/
	
	const auto& bravia = BraviaClient::create(manager, "192.168.1.116:20060");

	//const auto bravia{ manager.braviaClient("43.194.142.58:20060") };

	/*bravia->setVolume(12,
		[](const auto a, const bool& b, const int& c) 
	{
		printf("VOLUME WAS %s to %d\n", b ? "SET" : "NOT SET", c);
	});*/


	auto c = manager.createWebSocketServer("6502");
	c->onRecv = [&bravia](mg_connection* con, const auto& a)
	{
		
		bravia->setVolume(stoi(a), [con](const auto a, const bool& b, const int& c)
		{
			std::stringstream ss;
			ss << c;
			std::string s{ ss.str() };
			mg_connection* com = con;
			mg_send_websocket_frame(com, WEBSOCKET_OP_TEXT, s.c_str(), s.size());
		});
		return "";
	};

	std::string command = "{\"system\":{\"set_relay_state\":{\"state\":";
	command += "0";
	command += "}}}";

	auto plugClient = manager.createClient("192.168.1.8:9999");
	meerkat::utilities::autokeyEncrypt(command, 171);

	std::vector<char> sendBuffer;
	toBuffer(command.length(), sendBuffer);

	sendBuffer.insert(sendBuffer.end(), command.begin(), command.end());

	std::string sendStr{ sendBuffer.begin(), sendBuffer.end() };
	plugClient->send(sendStr);


	auto httpClient = manager.httpClient("192.168.1.27:80");
	//httpClient->send(HttpClient::Method::Put, "192.168.1.27", "/api/VPYfCwtxH3cFTphV3oIEt5Gz7fQYI6p8tNucFZoq/lights/4/state", "{\"on\":true}");

	static bool s_muteStatus = true;
	auto then = std::chrono::high_resolution_clock::now();
	for (;;)
	{
		manager.poll(10);

		auto diff = std::chrono::high_resolution_clock::now() - then;
		if (std::chrono::duration_cast<std::chrono::seconds>(diff) > std::chrono::seconds{ 5 })
		{
			std::random_device rd;  //Will be used to obtain a seed for the random number engine
			std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
			std::uniform_int_distribution<> dis(1, 25);


		//	cll.send(req);

			/*bravia->setVolume(dis(gen),
				[](const auto a, const bool& b, const int& c)
			{
				printf("VOLUME WAS %s to %d\n", b ? "SET" : "NOT SET", c);
			});*/

			/*bravia->getVolume([](const auto a, const bool& b, const int& c)
			{
				printf("VOLUME IS: %d\n", c);
			});

			bravia->getPowerStatus([](const auto a, const bool& success, const bool& powerStatus)
			{
				std::cout << std::boolalpha << "Command success: " << success <<  "Power Status: " << powerStatus;
			});*/

			//bravia->setPowerStatus(false, [](const auto a, const bool& success, const bool& powerStatus)
			//{
			//	std::cout << std::boolalpha << "Command success: " << success << "Power Status: " << powerStatus;
			//});
			
			//bravia->setAudioMute(s_muteStatus, [](const auto a, const bool& success, const bool& muteStatus)
			//{
			//	s_muteStatus = !s_muteStatus;
			//	std::cout << std::boolalpha << "Command success: " << success << "Mute Status: " << muteStatus;
			//});


		/*	bravia->setPictureMute(false, [](const auto a, const bool& success, const bool& muteStatus)
			{
				s_muteStatus = !s_muteStatus;
				std::cout << std::boolalpha << "Command success: " << success << "Mute Status: " << muteStatus;
			});*/


			then = std::chrono::high_resolution_clock::now();
		}
	}

	return 0;
}