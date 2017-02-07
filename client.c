#include "client.h"
#include "error.h"
#include <assert.h>

struct Client* client_ctor(void* userdata)
{
	const SDL_version *link_version = SDLNet_Linked_Version();
	SDL_version compile_version;
	SDL_NET_VERSION(&compile_version);
	if(compile_version.major != link_version->major ||
		compile_version.minor != link_version->minor ||
		compile_version.patch != link_version->patch)
	{
		printf(
			"Warning: Program was compiled with SDL_image "
			"version %i.%i.%i, but was linked with version %i.%i.%i\n",
			compile_version.major,
			compile_version.minor,
			compile_version.patch,
			link_version->major,
			link_version->minor,
			link_version->patch
		);
	}

	if(SDLNet_Init())
		debug(SDLNet_GetError(), ERRORTYPE_CRITICAL);

	struct Client* self = malloc(sizeof(struct Client));
	if(!self)
		error("malloc", ERRORTYPE_MEMALLOC);
	
	*self = (struct Client){0};
	self->userdata = userdata;
	return self;
}

int client_inittcp(
	struct Client* self, 
	const char* ip,
	int port,
	int maxbufsize,
	struct ClientTCPEventHandlers handlers
)
{
	assert(self);
	assert(!(self->type & CLIENTTYPE_TCP));
	assert(port < (1 << 16));
	assert(maxbufsize > 0);

	if(SDLNet_ResolveHost(&self->tcpaddress, ip, port))
		debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);

	self->tcpsocket = SDLNet_TCP_Open(&self->tcpaddress);
	if(!self->tcpsocket)
	{
		//debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);
		puts(SDLNet_GetError());
		return 0;
	}

	self->tcpbuffer = malloc(maxbufsize);
	if(!self->tcpbuffer)
		debug("malloc", ERRORTYPE_MEMALLOC);
	self->tcpsocketset = SDLNet_AllocSocketSet(1);
	if(!self->tcpsocketset)
		debug("SDLNet_AllocSocketSet", ERRORTYPE_MEMALLOC);

	SDLNet_TCP_AddSocket(self->tcpsocketset, self->tcpsocket);

	self->type |= CLIENTTYPE_TCP;
	self->tcpip = ip;
	self->tcpport = port;
	self->tcphandlers = handlers;
	self->tcpmaxbufsize = maxbufsize;
	return 1;
}

int client_initudp(
	struct Client* self, 
	const char* ip,
	int port, 
	int maxbufsize,
	ClientOnUDPData ondata
)
{
	assert(self);
	assert(!(self->type & CLIENTTYPE_UDP));
	assert(port < (1 << 16));
	assert(maxbufsize > 0);

	if(SDLNet_ResolveHost(&self->udpaddress, ip, port))
		debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);
	
	self->udpsocket = SDLNet_UDP_Open(port);
	if(!self->udpsocket)
	{
		//debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);
		puts(SDLNet_GetError());
		return 0;
	}
	self->udppacket.data = malloc(maxbufsize);
	if(!self->udppacket.data)
		debug("malloc", ERRORTYPE_MEMALLOC);

	self->udppacket.maxlen = maxbufsize;
	self->udppacket.len = maxbufsize;
	SDLNet_UDP_Bind(self->udpsocket, 0, &self->udpaddress);

	self->type |= CLIENTTYPE_UDP;
	self->udphandler = ondata;
	self->udpport = port;
	self->udpip = ip;
	return 1;
}

void client_update(struct Client* self)
{
	assert(self);
}

void client_dtor(struct Client* self)
{
	assert(self);
	if(self->type & CLIENTTYPE_UDP)
	{
		free(self->udppacket.data);
		SDLNet_UDP_Close(self->udpsocket);
	}

	if(self->type & CLIENTTYPE_TCP)
	{
		free(self->tcpbuffer);
		SDLNet_TCP_Close(self->tcpsocket);
		SDLNet_FreeSocketSet(self->tcpsocketset);
	}

	free(self);
}

