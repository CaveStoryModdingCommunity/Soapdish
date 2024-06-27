#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <string>
#include <sstream>
#include <cstdint>
#include <curl\curl.h>
#include "curl_connect.h"
#include "soaprun.h"

char* CurlURL;

void CurlInit()
{
	curl_global_init(CURL_GLOBAL_SSL); //only need SSL since soaprun already inits winsock
}
//stolen from https://gist.github.com/whoshuu/2dc858b8730079602044
//because the curl documentation wasn't super clear on how to do this
static size_t writeFunction(void* ptr, size_t size, size_t length, std::string* data)
{
	data->append((char*)ptr, size * length);
	return size * length;
}

//Mimics Pixel's code that deals with Shift JIS characters and any type of line ending
static std::string ReadLine(std::string str, size_t* start_index)
{
	size_t resume_index = *start_index;
	size_t i;
	for(i = 0; *start_index + i < str.length(); i++)
	{
		char c = str[*start_index + i];
		if (c == '\r')
		{
			resume_index = *start_index + i + 1;
			if (resume_index < str.length() && str[resume_index] == '\n')
				resume_index++;
			break;
		}
		else if (c == '\n')
		{
			resume_index = *start_index + i + 1;
			break;
		}
		if (IsSHIFT_JIS(c) && *start_index + i + 1 < str.length())
			i++;
	}
	std::string result = str.substr(*start_index, i);
	*start_index = resume_index;
	return result;
}

//Checks/returns if the given CURLcode is an error, and writes a relevant message to the soaprun debug log
static bool CheckAndLogResult(CURLcode err, const char* message)
{
	if (err != CURLE_OK)
	{
		LogMessage("x %s %s", message, curl_easy_strerror(err));
		return false;
	}
	LogMessage("o %s", message);
	return true;
}

bool __cdecl CurlConnect(ServerInfo* serverInfo, char* exit_param)
{
	bool status = false;
	CURL* curl;
	curl = curl_easy_init();
	if (curl == nullptr)
	{
		LogMessage("x curl");
		return false;
	}
	LogMessage("o curl");

	std::string response_string;
	if (CheckAndLogResult(curl_easy_setopt(curl, CURLOPT_URL, CurlURL), "url")
		&& CheckAndLogResult(curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L), "followlocation")
		&& CheckAndLogResult(curl_easy_setopt(curl, CURLOPT_USERAGENT, UserAgent), "useragent")
		&& CheckAndLogResult(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction), "writefunction")
		&& CheckAndLogResult(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string), "writedata")
		&& CheckAndLogResult(curl_easy_perform(curl), "perform"))
	{
		auto index = response_string.find("<html", 0);
		if (index >= 0)
		{
			auto _line = ReadLine(response_string, &index);
			
			//Pixel line
			_line = ReadLine(response_string, &index);
			if (_line.rfind("Pixel", 0) == 0)
			{
				//the "OK." check would go here
				char* line = _strdup(ReadLine(response_string, &index).c_str());

				//skip first item
				(void)strtok(line, "\t");

				//status
				char* tok = strtok(nullptr, "\t");
				serverInfo->status = (tok != nullptr && strlen(tok) == 4 && _stricmp(tok, "open") == 0);

				//ip
				tok = strtok(nullptr, "\t");
				memset(serverInfo->ip, 0, IP_BUFFER_SIZE);
				if (tok != nullptr)
				{
					index = strlen(tok);
					if (index < IP_BUFFER_SIZE)
					{
						memcpy(serverInfo->ip, tok, index);
					}
				}

				//port
				tok = strtok(nullptr, "\t");
				serverInfo->port = 0;
				if (tok != nullptr)
				{
					serverInfo->port = atol(tok);
				}

				//protocol
				tok = strtok(nullptr, "\t");
				memset(serverInfo->protocol, 0, PROTOCOL_BUFFER_SIZE);
				if (tok != nullptr)
				{
					index = strlen(tok);
					if (index < PROTOCOL_BUFFER_SIZE)
					{
						memcpy(serverInfo->protocol, tok, index);
					}
				}

				//version
				tok = strtok(nullptr, "\t");
				serverInfo->version = 0;
				if (tok != nullptr)
				{
					serverInfo->version = atol(tok);
				}

				//comments
				for (auto i = 0; i < COMMENT_COUNT; i++)
				{
					tok = strtok(nullptr, "\t");
					memset(serverInfo->comments[i], 0, COMMENT_BUFFER_SIZE);
					if (tok != nullptr)
					{
						index = strlen(tok);
						if (0 < index && (index < 4 || strcmp(tok, "<br>") != 0))
						{
							ReadURLEncodedString(tok, serverInfo->comments[i], COMMENT_BUFFER_SIZE);
						}
					}
				}
				free(line);

				status = true;
			}
		}
	}
	curl_easy_cleanup(curl);
	return status;
}