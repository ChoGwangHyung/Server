#pragma once
#pragma comment(lib,"ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<WinSock2.h>
#include<stdlib.h>
#include<iostream>
#include <fstream>

#define BUFFERSIZE 512
#define PORT 9000

class ConnectClientManager
{
private:
	//For Socket
	WSADATA Wsadata;
	USHORT SERVERPORT;
	SOCKET Listen_Socket;
	SOCKADDR_IN ServerAddress;

	//Variable for Data Communication
	SOCKET clientSocket;
	SOCKADDR_IN clientAddress;
	int AddressLen;
	char Buffer[BUFFERSIZE + 1];

	CRITICAL_SECTION cs;
public:
	ConnectClientManager();
	~ConnectClientManager();
	void error_Quit(const char *Msg);
	void error_Display(const char *Msg);
	int recvn(SOCKET s, char *buf, int len, int flags);
	void setReadyState();
	void communicate();
	static DWORD WINAPI SocketThread(LPVOID lpParam);
};