#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL_net.h>

enum ClientDisconnectReason
{
	CLIENTDISCONNECT_KICKED,
	CLIENTDISCONNECT_ERROR,
	CLIENTDISCONNECT_USER
};

typedef void(*ClientOnDisconnect)(enum ClientDisconnectReason, void*);

typedef void(*ClientOnTCPData)(size_t, char*, void*);
typedef void(*ClientOnUDPData)(IPaddress, size_t, char*, void*);

enum ClientType
{
	CLIENTTYPE_NONE = 0,
	CLIENTTYPE_TCP = 1 << 0,
	CLIENTTYPE_UDP = 1 << 1
};

struct ClientTCPEventHandlers
{
	ClientOnDisconnect tcpdisconnect;
	ClientOnTCPData tcpreceived;
};

struct Client
{
	struct ClientTCPEventHandlers tcphandlers;
	IPaddress tcpaddress;
	SDLNet_SocketSet tcpsocketset;
	TCPsocket tcpsocket;
	char* tcpbuffer;
	const char* tcpip;
	int tcpmaxbufsize;
	int tcpport;

	ClientOnUDPData udphandler;
	IPaddress udpaddress;
	UDPpacket udppacket;
	UDPsocket udpsocket;
	const char* udpip;
	int udpport;

	void* userdata;
	int type;
};

struct Client* client_ctor(struct Client* self, void* userdata);
int client_inittcp(
	struct Client* self, 
	const char* ip,
	int port,
	int maxbufsize,
	struct ClientTCPEventHandlers handlers
);

/* Will bind address to channel 0 on success */
int client_initudp(
	struct Client* self, 
	const char* ip,
	int port, 
	int maxbufsize,
	ClientOnUDPData ondata
);
void client_sendtcp(struct Client* self, void* buffer, size_t bufsize);
void client_sendudp(struct Client* self, void* buffer, size_t bufsize);
void client_update(struct Client* self);
void client_dtor(struct Client* self);

#endif
