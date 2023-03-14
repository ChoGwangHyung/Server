#include "ConnectClientManager.h"

ConnectClientManager::ConnectClientManager()
{
	SERVERPORT = PORT;
	if (WSAStartup(MAKEWORD(2, 2), &Wsadata) != 0)
		return;

	InitializeCriticalSection(&cs);
}

ConnectClientManager::~ConnectClientManager()
{
	//CloseSocket()
	closesocket(Listen_Socket);

	//WIndsock Quit
	WSACleanup();
}

//Displaying Socket Error
void ConnectClientManager::error_Quit(const char *Msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)Msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//Displaying Socket Function Error
void ConnectClientManager::error_Display(const char *Msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << "[" << Msg << "]" << (TCHAR *)lpMsgBuf;
	LocalFree(lpMsgBuf);
}

int ConnectClientManager::recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}

void ConnectClientManager::setReadyState()
{
	int Retval;

	//Socket()
	Listen_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Listen_Socket == INVALID_SOCKET)
		error_Quit("Socket()");

	//bind()
	ZeroMemory(&ServerAddress, sizeof(SOCKADDR_IN));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ServerAddress.sin_port = htons(SERVERPORT);
	Retval = bind(Listen_Socket, (SOCKADDR *)&ServerAddress, sizeof(ServerAddress));
	if (Retval == SOCKET_ERROR)
		error_Quit("Bind()");

	//listen()
	Retval = listen(Listen_Socket, SOMAXCONN);
	if (Retval == SOCKET_ERROR)
		error_Quit("listen()");
}

void ConnectClientManager::communicate()
{
	HANDLE hThread;

	while (1)
	{
		//Accept()
		AddressLen = sizeof(clientAddress);
		clientSocket = accept(Listen_Socket, (SOCKADDR *)&clientAddress, &AddressLen);
		if (clientSocket == INVALID_SOCKET)
		{
			error_Display("Accept");
			break;
		}

		//Displaying Client Display
		std::cout << std::endl << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddress.sin_addr) << ", ��Ʈ ��ȣ = " << ntohs(clientAddress.sin_port) << std::endl;

		//Create Thread
		hThread = CreateThread(NULL, 0, SocketThread, this, 0, NULL);
		if (hThread == NULL)
			closesocket(clientSocket);
		else
			CloseHandle(hThread);
	}
}

DWORD WINAPI ConnectClientManager::SocketThread(LPVOID lpParam)
{
	ConnectClientManager*This = (ConnectClientManager*)lpParam;
	SOCKET sock = This->clientSocket;
	int retval;
	int addressLen;
	SOCKADDR_IN threadSocketAddress;
	char Buffer[BUFFERSIZE + 1];

	//Get Client Information
	addressLen = sizeof(SOCKADDR_IN);
	getpeername(sock, (SOCKADDR *)&threadSocketAddress, &addressLen);

	while (1) {
		//�޴� ������ �ޱ�	
		int menuNum;
		retval = This->recvn(sock, (char*)&menuNum, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			This->error_Display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		//���� ������
		if (menuNum == 1) {
			std::ifstream fin;
			int fileLen = 0;

			fin.open("test.txt");

			if (fin.is_open()) {
				char temp;
				while (fin.get(temp)) {
					Buffer[fileLen++] = temp;
				}
			}
			fin.close();

			//���ڿ� ũ�� ����
			retval = send(sock, (char*)&fileLen, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				This->error_Display("send()");
				break;
			}

			//���ڿ� ������
			retval = send(sock, Buffer, fileLen, 0);
			if (retval == SOCKET_ERROR) {
				This->error_Display("send()");
				break;
			}
			printf("[TCP/%s:%d] ���� ���� ����\n", inet_ntoa(threadSocketAddress.sin_addr),
				   ntohs(threadSocketAddress.sin_port));

		}
		//���� ����
		else {
			//�����ڿ� ����
			EnterCriticalSection(&This->cs);

			int messageLen;
			// message �ޱ�(ũ��)
			retval = This->recvn(sock, (char*)&messageLen, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				This->error_Display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// message �ޱ�(������)
			retval = This->recvn(sock, Buffer, messageLen, 0);
			if (retval == SOCKET_ERROR) {
				This->error_Display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			Buffer[retval] = '\0';
			std::ofstream fout;
			fout.open("test.TXT", std::ios::app);

			if (fout.is_open()) {
				fout.write(Buffer, messageLen);
			}

			printf("[TCP/%s:%d �߰�] %s\n", inet_ntoa(threadSocketAddress.sin_addr),
				   ntohs(threadSocketAddress.sin_port), Buffer);

			fout.close();

			//�����ڿ� Ż��
			LeaveCriticalSection(&This->cs);
		}
	}
	closesocket(sock);
	std::cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ�=" << inet_ntoa(threadSocketAddress.sin_addr) << "��Ʈ ��ȣ = " << ntohs(threadSocketAddress.sin_port) << std::endl << std::endl;

	return 1;
}