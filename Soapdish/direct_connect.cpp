#define _CRT_SECURE_NO_WARNINGS
#include "direct_connect.h"
#include <cstring>

char* DirectIP;
int32_t DirectPort;

bool __fastcall DirectConnect(SoaprunConnection* connection)
{
	//no validity checks because the parsing function already did that
	size_t len = strlen(DirectIP);
	strncpy(&connection->serverIP[0], DirectIP, len);
	connection->serverIP[len] = '\0';

	connection->serverPort = DirectPort;

	return true;
}