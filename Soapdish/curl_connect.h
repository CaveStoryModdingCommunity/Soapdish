#pragma once
#include "soaprun.h"

extern char* CurlURL;

void CurlInit();

bool __cdecl CurlConnect(ServerInfo* serverInfo, char* exit_param);