#include "ConnectServerManager.h"

ConnectServerManager::ConnectServerManager(char *ipAddress, int port)
{
	if (WSAStartup(MAKEWORD(2, 2), &Wsadata) != 0)
		return;
	SERVERPORT = port;
	serverIp = ipAddress;
}

ConnectServerManager::~ConnectServerManager()
{
	//closesocket()	
	closesocket(Socket);

	//Terminate Winsock
	WSACleanup();
}

//Displaying SOcket Function Error
void ConnectServerManager::error_Display(const char *Msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << "[" << Msg << "]" << (char *)lpMsgBuf;
	LocalFree(lpMsgBuf);
}

//Displaying Socket Function Error and then Terminate Program
void ConnectServerManager::error_Quit(const char *Msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)Msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void ConnectServerManager::getReadyState()
{
	//Socket()
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET)
	{
		error_Quit("socket()");
	}

	//connet()
	SOCKADDR_IN ServerAddress;
	ZeroMemory(&ServerAddress, sizeof(SOCKADDR_IN));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = inet_addr(serverIp.c_str());
	ServerAddress.sin_port = htons(SERVERPORT);

	if ((connect(Socket, (SOCKADDR *)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR))
	{
		error_Quit("connect()");
	}
}

//User Define Data Receving Function
int ConnectServerManager::recvn(SOCKET s, char *buf, int len, int flags)
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

void ConnectServerManager::communicate()
{
	//Variable for Data Communication
	char Buffer[BUFFERSIZE + 1];
	int Len;
	int Retval;

	//Communication with Server
	while (1)
	{
		int menuNum;
		printf("[TCP Ŭ���̾�Ʈ] �޴� ���� [1. �б�] [2. ����] [3.����] : ");
		scanf("%d", &menuNum);

		//�б�
		if (menuNum == 1) {
			//�޴� ����
			Retval = send(Socket, (char*)&menuNum, sizeof(int), 0);
			if (Retval == SOCKET_ERROR) {
				error_Display("send()");
				break;
			}

			int dataLen;
			// ������ �ޱ�(ũ��)
			Retval = recvn(Socket, (char*)&dataLen, sizeof(int), 0);
			if (Retval == SOCKET_ERROR) {
				error_Display("recv()");
				break;
			}
			else if (Retval == 0)
				break;

			// ������ �ޱ�
			Retval = recvn(Socket, Buffer, dataLen, 0);
			if (Retval == SOCKET_ERROR) {
				error_Display("recv()");
				break;
			}
			else if (Retval == 0)
				break;

			Buffer[Retval] = '\0';
			printf("[���� ������] %s\n", Buffer);
		}
		//����
		else if (menuNum == 2) {
			//�޴� ����
			Retval = send(Socket, (char*)&menuNum, sizeof(int), 0);
			if (Retval == SOCKET_ERROR) {
				error_Display("send()");
				break;
			}

			char message[BUFFERSIZE];
			printf("[���� ������] ");
			scanf("%s", message);

			// ������ �Է�(�ùķ��̼�)
			int len = strlen(message);
			strncpy(Buffer, message, len);

			// '\n' ���� ����
			int bufLen = strlen(Buffer);
			if (Buffer[len - 1] == '\n')
				Buffer[len - 1] = '\0';
			if (strlen(Buffer) == 0)
				break;

			//���ڿ� ũ�� ����
			Retval = send(Socket, (char*)&len, sizeof(int), 0);
			if (Retval == SOCKET_ERROR) {
				error_Display("send()");
				break;
			}

			//���ڿ� ������
			Retval = send(Socket, Buffer, len, 0);
			if (Retval == SOCKET_ERROR) {
				error_Display("send()");
				break;
			}
			printf("[TCP Ŭ���̾�Ʈ] %d������ ���ڿ��� ����ϴ�.\n", len);
		}
		//����
		else if (menuNum == 3) {
			printf("����");
			break;
		}
		else {
			printf("�߸��� �޴��Դϴ�. ���Է¹ٶ��ϴ�.\n");
			continue;
		}
	}
}