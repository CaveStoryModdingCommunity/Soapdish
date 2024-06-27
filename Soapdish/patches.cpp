#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <SimpleIni.h>
#include "soaprun.h"
#include "curl_connect.h"
#include "direct_connect.h"

//INI names
#define INI_PATH "server_config.ini"

#define CONNECTION_METHOD "connection_method"
#define ORIGINAL "original"
#define DIRECT "direct"
#define CURL_CATEGORY "curl"

#define USER_AGENT "user_agent"

#define DISPATCH_HOSTNAME "dispatch_hostname"
#define DISPATCH_PATH "dispatch_path"
#define DISPATCH_NAME "dispatch_name"
#define DISPATCH_PORT "dispatch_port"

#define DIRECT_IP "direct_ip"
#define DIRECT_PORT "direct_port"

#define CURL_URL "curl_url"

//Dispatch Variables
static char* userAgent;
static char* dispatchHostname;
static char* dispatchPath;
static char* dispatchName;

#define INI_NOT_FOUND "Config file not found! Make sure " INI_PATH " exists and that your working directory is your Soaprun directory."

#define CONNECTION_METHOD_INVALID "Invalid connection method provided! It should either be " ORIGINAL ", " DIRECT ", or " CURL_CATEGORY " (case sensitive)."
#define CONNECTION_METHOD_NOT_FOUND "No connection method found! Make sure " CONNECTION_METHOD " exists in " INI_PATH "."

#define DIRECT_IP_NOT_FOUND "IP not found! Make sure " DIRECT_IP " exists in the " DIRECT " category in " INI_PATH "."
#define DIRECT_IP_TOO_LONG "IP is too long! Keep it at or below 63 characters."
#define DIRECT_PORT_NOT_FOUND "Port not found! Make sure " DIRECT_PORT " exists in the " DIRECT " category in " INI_PATH "."
#define DIRECT_PORT_INVALID "Port is invalid! Keep it in the range 0-65535 inclusive."

#define ORIGINAL_HOSTNAME_NOT_FOUND "Hostname not found! Make sure " DISPATCH_HOSTNAME " exists in the " ORIGINAL " category in " INI_PATH "."
#define ORIGINAL_PATH_NOT_FOUND "Path not found! Make sure " DISPATCH_PATH " exists in the " ORIGINAL " category in " INI_PATH "."
#define ORIGINAL_NAME_NOT_FOUND "Name not found! Make sure " DISPATCH_NAME " exists in the " ORIGINAL " category in " INI_PATH "."
#define ORIGINAL_PORT_NOT_FOUND "Port not found! Make sure " DISPATCH_PORT " exists in the " ORIGINAL " category in " INI_PATH "."

#define CURL_URL_NOT_FOUND "URL not found! Make sure " CURL_URL " exists in the " CURL_CATEGORY " category in " INI_PATH "."

bool applyPatches()
{
	const HANDLE handle = GetCurrentProcess();
	CSimpleIniA ini;
	SI_Error rc = ini.LoadFile(INI_PATH);
	if (rc < 0)
	{
		MessageBoxA(nullptr, INI_NOT_FOUND, "INI Load Error", 0);
		return false;
	}
	size_t len;
	const char* buff;

	buff = ini.GetValue("", CONNECTION_METHOD);
	if (buff == nullptr)
	{
		MessageBoxA(nullptr, CONNECTION_METHOD_NOT_FOUND, "Connection Method Not Found Error", 0);
		return false;
	}
	if (strlen(buff) == strlen(DIRECT) && strcmp(buff, DIRECT) == 0)
	{
		buff = ini.GetValue(DIRECT, DIRECT_IP);
		if (buff == nullptr)
		{
			MessageBoxA(nullptr, DIRECT_IP_NOT_FOUND, "IP Not Found Error", 0);
			return false;
		}
		len = strlen(buff);
		if (len >= IP_BUFFER_SIZE)
		{
			MessageBoxA(nullptr, DIRECT_IP_TOO_LONG, "IP Too Long Error", 0);
			return false;
		}
		DirectIP = new char[len + 1]();
		strncpy(DirectIP, buff, len);

		if (ini.GetValue(DIRECT, DIRECT_PORT) == nullptr)
		{
			MessageBoxA(nullptr, DIRECT_PORT_NOT_FOUND, "Port Not Found Error", 0);
			return false;
		}
		long port = ini.GetLongValue(DIRECT, DIRECT_PORT);
		if (port < 0 || UINT16_MAX < port)
		{
			MessageBoxA(nullptr, DIRECT_PORT_INVALID, "Port Invalid Error", 0);
			return false;
		}
		DirectPort = port;

		//overwriting the spot that calls the dispatch server checker
		const HANDLE handle = GetCurrentProcess();
		const auto relative_address = (unsigned int)&DirectConnect - (0x40c3ea + 5);
		WriteProcessMemory(handle, (void*)(0x40c3ea + 1), &relative_address, 4, NULL);
		
		//Exit early since we won't be using any other options
		return true;
	}
	
	if (strlen(buff) == strlen(ORIGINAL) && strcmp(buff, ORIGINAL) == 0)
	{
		buff = ini.GetValue(ORIGINAL, DISPATCH_HOSTNAME);
		if (buff == nullptr)
		{
			MessageBoxA(nullptr, ORIGINAL_HOSTNAME_NOT_FOUND, "Hostname Not Found Error", 0);
			return false;
		}
		len = strlen(buff);
		dispatchHostname = new char[len + 1]();
		strncpy(dispatchHostname, buff, len);
		
		buff = ini.GetValue(ORIGINAL, DISPATCH_PATH);
		if (buff == nullptr)
		{
			MessageBoxA(nullptr, ORIGINAL_PATH_NOT_FOUND, "Path Not Found Error", 0);
			return false;
		}
		len = strlen(buff);
		dispatchPath = new char[len + 1]();
		strncpy(dispatchPath, buff, len);

		buff = ini.GetValue(ORIGINAL, DISPATCH_NAME);
		if (buff == nullptr)
		{
			MessageBoxA(nullptr, ORIGINAL_NAME_NOT_FOUND, "Name Not Found Error", 0);
			return false;
		}
		len = strlen(buff);
		dispatchName = new char[len + 1]();
		strncpy(dispatchName, buff, len);

		if (ini.GetValue(ORIGINAL, DISPATCH_PORT) == nullptr)
		{
			MessageBoxA(nullptr, ORIGINAL_PORT_NOT_FOUND, "Port Not Found Error", 0);
			return false;
		}
		long port = ini.GetLongValue(ORIGINAL, DISPATCH_PORT);
		if (port < 0 || UINT16_MAX < port)
		{
			MessageBoxA(nullptr, DIRECT_PORT_INVALID, "Port Invalid Error", 0);
			return false;
		}
		
		DispatchHostname = dispatchHostname;
		DispatchPath = dispatchPath;
		DispatchName = dispatchName;
		DispatchPort = (uint16_t)port;
	}
	else if (strlen(buff) == strlen(CURL_CATEGORY) && strcmp(buff, CURL_CATEGORY) == 0)
	{
		buff = ini.GetValue(CURL_CATEGORY, CURL_URL);
		if (buff == nullptr)
		{
			MessageBoxA(nullptr, CURL_URL_NOT_FOUND, "URL Not Found Error", 0);
			return false;
		}
		len = strlen(buff);
		CurlURL = new char[len + 1]();
		strncpy(CurlURL, buff, len);

		CurlInit();

		const auto relative_address = (unsigned int)&CurlConnect - (0x4061ff + 5);
		WriteProcessMemory(handle, (void*)(0x4061ff + 1), &relative_address, 4, NULL);
	}
	else
	{
		MessageBoxA(nullptr, CONNECTION_METHOD_INVALID, "Invalid Connection Method Error", 0);
		return false;
	}

	//Let people modify the user agent if they really want I guess
	buff = ini.GetValue("", USER_AGENT);
	if (buff != nullptr)
	{
		len = strlen(buff);
		userAgent = new char[len + 1]();
		strncpy(userAgent, buff, len);

		UserAgent = userAgent;
	}
	
	return true;
}