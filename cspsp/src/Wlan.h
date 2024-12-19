#ifndef _WLAN_H_
#define _WLAN_H_

#include "JGE.h"
#include <stdio.h> // For perror() call
#include <stdlib.h> // For exit() call
#include <iostream>
#include <psp2/sysmodule.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>

// #define bzero(p, l) memset(p, 0, l)
// #define bcopy(s, t, l) memmove(t, s, l)


#include <vector>

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_PICK 100

struct ConnectionConfig {
	char name[64];
	int index;
};

class Socket
{
public:
	int sock;
	struct sockaddr_in addrTo;
	struct sockaddr_in addr;
	bool serverSocket;
	//Socket():  {};
};


static const char* wlanNotInitialized = "WLAN not initialized.";
static bool wlanInitialized = false;
static char resolverBuffer[1024];
static int resolverId;

int WlanInit();
int WlanTerm();
std::vector<ConnectionConfig> GetConnectionConfigs();
int UseConnectionConfig(int config);
int GetConnectionState(int state);

char* GetIPAddress();
int SocketFree(Socket* socket);
int SetSockNoBlock(int s, u32 val);
int SocketConnect(Socket* socket, char* host, int port);
int SocketConnectUdp(Socket* socket, char* host, int port);
int SocketRecv(Socket* socket, char* buf, int size);
//sockaddr SocketRecvfrom(Socket* socket, char* buf, int size);
int SocketSend(Socket* socket, char* buf, int size);
int SocketSendUdp(Socket* socket, char* buf, int size);
int SocketClose(Socket* socket);

#endif
