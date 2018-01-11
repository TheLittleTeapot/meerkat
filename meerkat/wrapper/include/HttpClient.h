#pragma once
#include "../include/Connection.h"
#include <sstream>

class HttpClient : public Connection
{
public:
	HttpClient(mg_connection& con);
	virtual ~HttpClient();

	HttpClient(const HttpClient&) = delete;
	HttpClient(HttpClient&&) = delete;
	HttpClient& operator=(const HttpClient&) = delete;
	HttpClient& operator=(HttpClient&&) = delete;

	enum class Method
	{
		Get,
		Post,
		Put,
		Delete,
	};

	bool send(mg_connection& con, Method method, const std::string& host, const std::string path, const std::string& postData = {})
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


		
		//mg_printf()
		return false;
	}

private:
	virtual void eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data) override;
};