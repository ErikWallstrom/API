#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL_net.h>
#include "vec.h"

struct Client;
struct Server;

enum ServerDisconnectReason
{
	SERVERDISCONNECT_KICKED,
	SERVERDISCONNECT_ERROR,
	SERVERDISCONNECT_USER
};

/* return 1 if client is accepted */
typedef int(*ServerOnConnect)(struct Client*, void*);
/* return 0 if client should be removed */
typedef int(*ServerOnTCPData)(struct Client*, size_t, char*, void*);
typedef void(*ServerOnDisconnect)(
	struct Client*,
	enum ServerDisconnectReason, 
	void*
);
typedef int(*ServerOnUDPData)(IPaddress, size_t, char*, void*);

enum ServerType 
{
	SERVERTYPE_TCP = 1 << 0,
	SERVERTYPE_UDP = 1 << 1
};

struct ServerEventHandlers
{
	ServerOnConnect tcpconnect;
	ServerOnDisconnect tcpdisconnect;
	ServerOnTCPData tcpreceived;

	ServerOnUDPData udpreceived;
};

struct Client
{
	IPaddress address;
	void* userdata;

	/* Not mandatory */
	TCPsocket socket;
};

struct Server
{
	struct ServerEventHandlers eventhandlers;
	IPaddress address;
	/* Union-ish */
	struct {
		UDPpacket packet;
		UDPsocket udp;

		SDLNet_SocketSet socketset;
		TCPsocket tcp; 
		char* buffer;
		int maxbufsize;
	};

	Vec(struct Client) clients;
	int maxclients;

	void* userdata;
	enum ServerType type;
	Uint16 port;
};

struct Server* server_ctor(int port, int maxclients, int maxbufsize, void* userdata, struct ServerEventHandlers eventhandlers, enum ServerType type);
void server_update(struct Server* self);
void server_dtor(struct Server* self);

#endif
