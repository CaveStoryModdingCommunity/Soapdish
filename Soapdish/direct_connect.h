#pragma once
#include <cstdint>
#include "soaprun.h"

extern char* DirectIP;
extern int32_t DirectPort;

bool __fastcall DirectConnect(SoaprunConnection* connection);