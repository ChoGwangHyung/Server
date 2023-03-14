#include "ConnectServerManager.h"

int main(int argc, char *argv[])
{
	ConnectServerManager ConnectManager(argv[1], atoi(argv[2]));
	ConnectManager.getReadyState();
	ConnectManager.communicate();
}