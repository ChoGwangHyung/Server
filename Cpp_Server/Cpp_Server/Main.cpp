#include "ConnectClientManager.h"

int main(int argc, char *argv[])
{
	ConnectClientManager ConnectManager;
	ConnectManager.setReadyState();
	ConnectManager.communicate();

	return 0;
}