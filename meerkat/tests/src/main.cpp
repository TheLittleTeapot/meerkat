#include "mg_manager.h"

#include <sstream>
#include <iomanip>

void braviaControlTest();
void ssdpTest();
void wifiPlug();

int main(void)
{
	//ssdpTest();
	//braviaControlTest();
	wifiPlug();
}

void braviaControlTest()
{
	meerkat::Manager manager;

	const auto wkClient = manager.connect("192.168.1.116:20060");

	//const auto wkServer = manager.bind("1234");

	/*if (auto shrdServer = wkServer.lock())
	{
	shrdServer->m_onReceive = [](meerkat::Connection& connection, const meerkat::Buffer& buff)
	{
	std::string str{ buff.begin(), buff.end() };
	printf("Server received: %s\n", str.c_str());

	if (str == "ping")
	connection.send("pong");
	};

	shrdServer->m_onSend = [](int bytes){ printf("Server sent %d bytes\n", bytes); };
	}*/

	if (auto sharedBraviaConnection = wkClient.lock())
	{
		sharedBraviaConnection->m_onConnect = [](int result) {printf("Connected result: %d\n", result); };
		sharedBraviaConnection->m_onReceive = [](meerkat::Connection& conn, const meerkat::Buffer& buff)
		{
			std::string str{ buff.begin(), buff.end() };
			printf("Client received: %s\n", str.c_str());

			 conn.m_connection.flags |= MG_F_CLOSE_IMMEDIATELY;
		};

		sharedBraviaConnection->m_onClose = []() 
		{
			printf("Connection Closed\n"); 
		};

	}

	auto then = std::chrono::high_resolution_clock::now();
	for (;;)
	{
		manager.poll(std::chrono::milliseconds{ 10 });

		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - then).count() > 5)
		{
			const std::string togglePicuteMute = "*SCTPMU################\n";
			const std::string setVolume = "*SCVOLU0000000000000029\n";
			const std::string getVolume = "*SEVOLU################\n";
			const std::string getInput = "*SEINPT################\n";

			std::stringstream powerOn;
			powerOn << "*S";
			powerOn << "C";
			powerOn << "POWR";
			powerOn << std::setw(16) << std::setfill('0') << 0;
			powerOn << '\n';
			std::string powerOnStr = powerOn.str();

			static int targetVolume = 40;
			// targetVolume++;

			std::stringstream ss;
			ss << "*S";
			ss << "C";
			ss << "VOLU";
			ss << std::setw(16) << std::setfill('0') << targetVolume;
			ss << '\n';
			std::string setVolumeCustom = ss.str();


			if (auto connection = wkClient.lock())
				connection->send(togglePicuteMute);

			then = std::chrono::high_resolution_clock::now();
		}
	}
}

void ssdpTest()
{
	meerkat::Manager manager;

	const auto wkUdp = manager.connect("udp://239.255.255.250:1900");

	if (auto udpConnection = wkUdp.lock())
	{
		udpConnection->m_onConnect = [](int result) {printf("Connected result: %d\n", result); };
		udpConnection->m_onReceive = [](meerkat::Connection& conn, const meerkat::Buffer& buff)
		{
			std::string str{ buff.begin(), buff.end() };
			printf("Client received: %s\n", str.c_str());
		};

		std::stringstream searchstream;
		searchstream << "M-SEARCH * HTTP/1.1\r\n";
		searchstream << "HOST: 239.255.255.250:1900\r\n";
		searchstream << "MAN: \"ssdp:discover\"\r\n";
		searchstream << "MX: 1\r\n";
		searchstream << "ST: ssdp:all\r\n";

		udpConnection->send(searchstream.str());
	}

	auto then = std::chrono::high_resolution_clock::now();
	for (;;)
	{
		manager.poll(std::chrono::milliseconds{ 10 });

	}
}

//def encrypt(string) :
//	key = 171
//	result = "\0\0\0\0"
//	for i in string :
//a = key ^ ord(i)
//key = a
//result += chr(a)
//return result

std::string encrypt2(std::string& in, int keyIn)
{
	int key = keyIn;
	std::string output = "";
	for (auto chr : in)
	{
		int encryptedChar = (chr ^ key);
		key = encryptedChar;
		output += (char)encryptedChar;
	}
	return output;
}

void
toBuffer(unsigned int value, std::vector<char>& buffer)
{
	buffer.push_back((value >> 24) & 0xFF);
	buffer.push_back((value >> 16) & 0xFF);
	buffer.push_back((value >> 8) & 0xFF);
	buffer.push_back((value) & 0xFF);
}

std::string makePlugMessage(std::string& in, int key)
{
	auto body = encrypt2(in, key);

	std::vector<char> message;
	toBuffer(body.size(), message);

	message.insert(message.end(), body.begin(), body.end());

	return{ message.begin(), message.end() };
}

std::string decryptPlugMessage(std::vector<char> buffer)
{
	auto minusHeader = { buffer.begin() + 4, buffer.end() };

	
}

void wifiPlug()
{
	meerkat::Manager manager;

	const auto wkClient = manager.connect("192.168.1.8:9999");

	if (auto sharedBraviaConnection = wkClient.lock())
	{
		sharedBraviaConnection->m_onConnect = [](int result) {printf("Connected result: %d\n", result); };
		sharedBraviaConnection->m_onReceive = [](meerkat::Connection& conn, const meerkat::Buffer& buff)
		{
			std::string str{ buff.begin(), buff.end() };
			printf("Client received: %s\n", str.c_str());
		};

		sharedBraviaConnection->m_onClose = []()
		{
			printf("Connection Closed\n");
		};

	}

	auto then = std::chrono::high_resolution_clock::now() - std::chrono::seconds{ 5 };
	for (;;)
	{
		manager.poll(std::chrono::milliseconds{ 10 });

		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - then).count() > 5)
		{
			//std::string command = "{\"system\":{\"set_relay_state\":{\"state\":0}}}";
			//std::string command = "{\"system\":{\"get_sysinfo\":{}}}";
			std::string command = "{\"system\":{\"get_sysinfo\":null}}";
			auto toSend = makePlugMessage(command, 171);
			if (auto connection = wkClient.lock())
				connection->send(toSend);

			then = std::chrono::high_resolution_clock::now();
		}
	}
}
