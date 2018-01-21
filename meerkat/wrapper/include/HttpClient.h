#pragma once
#include "../include/Connection.h"
#include <sstream>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <vector>


enum class Method
{
	Head,
	Get,
	Post,
	Put,
	Patch,
	Delete,
};




class HttpClient : public Connection
{
public:
	HttpClient(mg_connection& con);
	virtual ~HttpClient();

	HttpClient(const HttpClient&) = delete;
	HttpClient(HttpClient&&) = delete;
	HttpClient& operator=(const HttpClient&) = delete;
	HttpClient& operator=(HttpClient&&) = delete;

	bool send(mg_connection& con, Method method, const std::string& host, const std::string path, const std::string& postData = {});


	

private:
	virtual void eventHandler(mg_connection& mgCon, const ConnectionEvent evt, void* data) override;
};


