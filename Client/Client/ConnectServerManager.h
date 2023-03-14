#pragma once
#pragma comment(lib,"ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<WinSock2.h>
#include<stdlib.h>
#include<iostream>

#define BUFFERSIZE 512

class ConnectServerManager
{
private:
	WSADATA Wsadata;	//initiate Winsock
	SOCKET Socket;
	int SERVERPORT;
	std::string serverIp;
public:
	ConnectServerManager(char *IPAddress, int port);
	~ConnectServerManager();
	void error_Display(const char *Msg);
	void error_Quit(const char *Msg);
	int recvn(SOCKET s, char *buf, int len, int flags);
	void getReadyState();
	void communicate();
};