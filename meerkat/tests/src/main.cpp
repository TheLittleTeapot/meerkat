#include "mg_manager.h"
#include "../../home_automation_server/include/utilities/autokey_cypher.h"

#include <sstream>
#include <iomanip>

void braviaControlTest();
void ssdpTest();
void udpDiscovery();
void wifiPlug();
void toBuffer(unsigned int value, std::vector<char>& buffer);
std::string decryptPlugMessage(std::vector<char> buffer);

std::string encrypt2(std::string& in, int keyIn);

void uint32BE(const uint32_t& value,const uint32_t& base, std::vector<char>& buffer)
{
	int remainder = value;
	while (remainder != 0)
	{
		auto remaining = (remainder / base);
		buffer.emplace_back(remainder % base);
		remainder = remaining;
	}
}

int main(void)
{
	//ssdpTest();
	//braviaControlTest();
	//wifiPlug();
	udpDiscovery();
	/*
	std::string theOriginalSring = "hello, World!";
	std::string transform = theOriginalSring;

	meerkat::utilities::autokeyEncrypt(transform, 171);
	meerkat::utilities::autokeyDecrypt(transform, 171);

	uint32_t length = 1045;
	meerkat::Buffer b, c;
	toBuffer(length, b);
	uint32BE(length, 256, c);
	*/
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

void udpDiscovery()
{
	meerkat::Manager manager;
	const auto wkDiscovery = manager.connect("udp://192.168.0.255:9999");

	if (auto discovery = wkDiscovery.lock())
	{
		discovery->m_onConnect = [](int result) {printf("Connected result: %d\n", result); };
		discovery->m_onReceive = [](meerkat::Connection& conn, const meerkat::Buffer& buff)
		{
			std::string str{ buff.begin(), buff.end() };
			printf("Client received: %s\n", str.c_str());
		};

		std::string command = "{\"system\":{\"get_sysinfo\":{}}}";
		meerkat::utilities::autokeyEncrypt(command, 171); //makePlugMessage(command, 171);

		std::vector<char> message;
		toBuffer(command.size(), message);

		message.insert(message.end(), command.begin(), command.end());

		discovery->send(message);
	}

	for (;;)
	{
		manager.poll(std::chrono::milliseconds{ 10 });
	}
}

void ssdpTest()
{
	meerkat::Manager manager;

	const auto wkUdp = manager.connect("udp://239.255.255.250:9999");

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

void toBuffer(unsigned int value, std::vector<char>& buffer)
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

//if (typeof firstKey == = 'undefined') firstKey = 0x2B;
//var buf = new Buffer(input); // node v6: Buffer.from(input)
//var key = firstKey;
//var nextKey;
//for (var i = 0; i < buf.length; i++) {
//	nextKey = buf[i];
//	buf[i] = buf[i] ^ key;
//	key = nextKey;
//}
//return buf;

std::string decryptPlugMessage(std::vector<char> buffer)
{
	auto minusHeader = { buffer.begin() + 4, buffer.end() };

	int key = 171;//0x2B;
	std::string retStr;
	int nextKey;

	for (auto chr : buffer)
	{
		nextKey = chr;
		retStr += chr ^ key;
		key = nextKey;
	}

	return{};
	
}

void wifiPlug()
{
	meerkat::Manager manager;
	auto wkClient = manager.connect("192.168.1.8:9999");

	if (auto sharedBraviaConnection = wkClient.lock())
	{
		sharedBraviaConnection->m_onConnect = [](int result) {printf("Connected result: %d\n", result); };
		sharedBraviaConnection->m_onReceive = [](meerkat::Connection& conn, const meerkat::Buffer& buff)
		{
			std::string str{ buff.begin()+4, buff.end() };
			meerkat::utilities::autokeyDecrypt(str, 171);
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

		if(!wkClient.lock())
			wkClient = manager.connect("192.168.1.8:9999");

		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - then).count() > 5)
		{
			static bool a = true;
			
			std::string command = "{\"system\":{\"set_relay_state\":{\"state\":";
			command += a ? "1" : "0";
			command += "}}}";

			a = !a;

			//std::string command = "{\"system\":{\"get_sysinfo\":{}}}";
			//std::string command = "{\"system\":{\"get_sysinfo\":{}}}";

			meerkat::utilities::autokeyEncrypt(command, 171); //makePlugMessage(command, 171);

			std::vector<char> message;
			toBuffer(command.size(), message);

			message.insert(message.end(), command.begin(), command.end());

			if (auto connection = wkClient.lock())
				connection->send(message);

			then = std::chrono::high_resolution_clock::now();
		}
	}
}
