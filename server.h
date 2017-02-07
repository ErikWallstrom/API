#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL_net.h>
#include "vec.h"

enum ServerDisconnectReason
{
	SERVERDISCONNECT_KICKED,
	SERVERDISCONNECT_ERROR,
	SERVERDISCONNECT_USER
};

struct ServerTCPClient
{
	IPaddress address;
	TCPsocket socket;
	void* userdata;
};

/* return 1 if client is accepted */
typedef int(*ServerOnConnect)(struct ServerTCPClient*, void*);
typedef void(*ServerOnDisconnect)(
	struct ServerTCPClient*,
	enum ServerDisconnectReason, 
	void*
);

typedef void(*ServerOnUDPData)(IPaddress, size_t, char*, void*);
/* return 0 if client should be removed */
typedef int(*ServerOnTCPData)(
	struct ServerTCPClient*,
	size_t, 
	char*, 
	void*
);

struct ServerTCPEventHandlers
{
	ServerOnConnect tcpconnect;
	ServerOnDisconnect tcpdisconnect;
	ServerOnTCPData tcpreceived;
};

enum ServerType 
{
	SERVERTYPE_NONE = 0,
	SERVERTYPE_TCP = 1 << 0,
	SERVERTYPE_UDP = 1 << 1
};

struct Server
{
	struct ServerTCPEventHandlers tcphandlers;
	Vec(struct ServerTCPClient) tcpclients;
	IPaddress tcpaddress;
	SDLNet_SocketSet tcpsocketset;
	TCPsocket tcpsocket;
	char* tcpbuffer;
	int tcpmaxbufsize;
	int tcpmaxclients;
	int tcpport;

	ServerOnUDPData udphandler;
	UDPpacket udppacket;
	UDPsocket udpsocket;
	int udpport;

	void* userdata;
	int type;
};

struct Server* server_ctor(void* userdata);
int server_inittcp(
	struct Server* self, 
	int port,
	int maxbufsize,
	int maxclients,
	struct ServerTCPEventHandlers handlers
);
int server_initudp(
	struct Server* self, 
	int port, 
	int maxbufsize,
	ServerOnUDPData ondata
);
void server_update(struct Server* self);
void server_dtor(struct Server* self);

#endif
