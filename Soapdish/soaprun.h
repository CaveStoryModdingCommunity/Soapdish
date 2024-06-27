#pragma once
#include <cstdint>

static const char*& UserAgent = *((const char**)0x431284);
static const char*& DispatchHostname = *((const char**)0x43126c);
static const char*& DispatchPath = *((const char**)0x431270);
static const char*& DispatchName = *((const char**)0x431278);
static uint16_t& DispatchPort = *((uint16_t*)0x431274);

//Checks if the given character is the start of a Shift JIS multi-byte sequence
static bool (* const IsSHIFT_JIS)(char character) = (bool(*)(char))0x414570;
//Logs a message to the current Soaprun debug log
static void(* const LogMessage)(const char* format, ...) = (void(*)(const char*, ...))0x415290;
//Parses a URL encoded string
static bool(* const ReadURLEncodedString)(char* input, char* output, int32_t max) = (bool(*)(char*, char*, int32_t))0x429600;

#define IP_BUFFER_SIZE 64
//haven't bothered reversing every parameter, since we only need two
struct SoaprunConnection
{
	void* unknownClass;
	char serverIP[IP_BUFFER_SIZE];
	int32_t serverPort;
	char* buff2;
	char* buff3;
	int32_t statusThing;
};

#define PROTOCOL_BUFFER_SIZE 8
#define COMMENT_COUNT 6
#define COMMENT_BUFFER_SIZE 24
struct ServerInfo
{
	int32_t status;
	char ip[IP_BUFFER_SIZE];
	char protocol[PROTOCOL_BUFFER_SIZE]; //expecting "Soaprun"
	char comments[COMMENT_COUNT][COMMENT_BUFFER_SIZE];
	int32_t port;
	int32_t version;
};