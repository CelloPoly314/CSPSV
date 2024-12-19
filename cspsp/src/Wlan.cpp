#include "Wlan.h"
#include <cstring>



int WlanInit()
{
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	int ret = sceNetShowNetstat();
	SceNetInitParam initparam;
	if (ret == SCE_NET_ERROR_ENOTINIT) {
		initparam.memory = malloc(141 * 1024);
		initparam.size = 141 * 1024;
		initparam.flags = 0;
		sceNetInit(&initparam);
	}
	sceNetCtlInit();
	sceNetCtlInetGetState(&ret);
	if (ret != SCE_NETCTL_STATE_CONNECTED)
	{
		sceNetCtlTerm();
		return -1;
	}

	wlanInitialized = true;
	return 0;
}

int WlanTerm()
{
	if (!wlanInitialized) return 0;
	sceNetTerm();
	wlanInitialized = false;
	return 0;
}

int SocketFree(Socket* socket)
{
	sceNetSocketClose(socket->sock);
	delete socket;
	return 0;
}

int SetSockNoBlock(int s, u32 val)
{ 
	// unsigned long nonblocking = 1;
	// ioctlsocket(s, FIONBIO, &nonblocking);

    // return 1;
	// 如果 val 為非零，設置非阻塞模式；否則設置為阻塞模式
    if (val) {
        // 使用 fcntl 設置非阻塞模式
        int flags = fcntl(s, F_GETFL, 0);
        if (flags == -1) {
            return -1; // 無法獲取當前標誌
        }
        return fcntl(s, F_SETFL, flags | O_NONBLOCK);
    } else {
        // 設置為阻塞模式
        int flags = fcntl(s, F_GETFL, 0);
        if (flags == -1) {
            return -1; // 無法獲取當前標誌
        }
        return fcntl(s, F_SETFL, flags & ~O_NONBLOCK);
    }

    return 1;
}

int SocketConnect(Socket* socket1, char* host, int port)
{
	
	//if (!wlanInitialized) return 0;
	
	socket1->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (socket1->sock < 0) ;//error("socket");

	struct hostent *hp;
	hp = gethostbyname(host);

	socket1->addrTo.sin_family = AF_INET;
	socket1->addrTo.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)hp->h_addr));
	//memcpy(&socket1->addrTo.sin_addr.s_addr, hp->h_addr, hp->h_length);
	socket1->addrTo.sin_port = htons(port);

	SetSockNoBlock(socket1->sock, 1);

	connect(socket1->sock, (struct sockaddr *)&socket1->addrTo, sizeof socket1->addrTo);
	//int status = connect(socket1->sock, (struct sockaddr *)&socket1->addrTo, sizeof socket1->addrTo);

	return 1;
}

int SocketConnectUdp(Socket* socket1, char* host, int port)
{
	
	//if (!wlanInitialized) return 0;
	
	socket1->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket1->sock < 0) ;//error("socket");

	socket1->addrTo.sin_family = AF_INET;

	struct hostent *hp;
	hp = gethostbyname(host);

	//bcopy((char *)hp->h_addr, (char *)&socket1->addrTo.sin_addr, hp->h_length);
	memmove((char *)&socket1->addrTo.sin_addr, (char *)hp->h_addr, hp->h_length);

	socket1->addrTo.sin_port = htons(port);

	SetSockNoBlock(socket1->sock, 1);
	//unsigned long nonblocking = 1;
	//ioctlsocket(socket1->sock, FIONBIO, &nonblocking);

	return 1;
}

int SocketRecv(Socket* socket, char* buf, int size)
{
	//if (!wlanInitialized) {} //return 0;

	//if (socket->serverSocket) {}//return luaL_error(L, "recv not allowed for server sockets.");

	struct sockaddr_in addrFrom;
	unsigned int len = sizeof (addrFrom); 

	int count = recv(socket->sock, buf, size, 0);
	return count;
	/*if (count > 0) {
		return count;
	} else {
		return 0;
	}*/
}

int SocketSend(Socket* socket, char* buf, int size)
{
	//if (!wlanInitialized) return 0;

	int n = send(socket->sock,buf,size,0);

	return n;
}


int SocketSendUdp(Socket* socket, char* buf, int size)
{
	//if (!wlanInitialized) return 0;

    int total = 0;        // how many bytes we've sent
    int bytesleft = size; // how many we have left to send
    int n;

    while(total < size) {
        n = sendto(socket->sock, buf+total, bytesleft, 0, (sockaddr*)&(socket->addrTo),sizeof(socket->addrTo));
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

	return 1;
}

int SocketClose(Socket* socket)
{
	//if (!wlanInitialized) return 0;

	sceNetSocketClose(socket->sock);
	return 1;
}

