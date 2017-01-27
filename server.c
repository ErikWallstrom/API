#include "server.h"
#include "error.h"
#include <assert.h>

struct Server* server_ctor(
	int port, 
	int maxclients, 
	int maxbufsize, 
	void* userdata, 
	struct ServerEventHandlers eventhandlers, 
	enum ServerType type
)
{
	assert(port < (1 << 16));
	assert(maxbufsize > 0);
	assert(type == SERVERTYPE_TCP || type == SERVERTYPE_UDP);

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

	struct Server* self = malloc(sizeof(struct Server));
	if(!self)
		error("malloc", ERRORTYPE_MEMALLOC);
	
	self->port = port;
	self->type = type;
	self->userdata = userdata;
	self->maxclients = maxclients;
	self->maxbufsize = maxbufsize;
	self->eventhandlers = eventhandlers;
	self->clients = vec_ctor(sizeof(struct Client), maxclients);

	if(SDLNet_ResolveHost(&self->address, NULL, self->port))
		debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);

	if(type & SERVERTYPE_TCP)
	{
		self->tcp = SDLNet_TCP_Open(&self->address);
		if(!self->tcp)
			debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);
		self->buffer = malloc(maxbufsize);
		if(!self->buffer)
			debug("malloc", ERRORTYPE_MEMALLOC);
		self->socketset = SDLNet_AllocSocketSet(maxclients + 1);
		if(!self->socketset)
			debug("SDLNet_AllocSocketSet", ERRORTYPE_MEMALLOC);
		SDLNet_TCP_AddSocket(self->socketset, self->tcp);
	}
	else
	{
		self->tcp = NULL;
		self->buffer = NULL;
		self->socketset = NULL;
	}

	if(type & SERVERTYPE_UDP)
	{
		self->udp = SDLNet_UDP_Open(self->port);
		if(!self->udp)
			debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);
		if(self->buffer)
			self->packet.data = (Uint8*)self->buffer;
		else
		{
			self->packet.data = malloc(maxbufsize);
			if(!self->packet.data)
				debug("malloc", ERRORTYPE_MEMALLOC);
		}

		self->packet.maxlen = maxbufsize;
		self->packet.len = maxbufsize;
	}
	else
	{
		self->udp = NULL;
		self->packet = (UDPpacket){0};
	}

	return self;
}

void server_update(struct Server* self)
{
	assert(self);
	if(self->type & SERVERTYPE_TCP)
	{
		SDLNet_CheckSockets(self->socketset, 0);
		TCPsocket newclient = SDLNet_TCP_Accept(self->tcp);
		if(newclient)
		{
			struct Client client = (struct Client){
				.socket = newclient,
				.address = *SDLNet_TCP_GetPeerAddress(newclient)
			};
			if(self->eventhandlers.tcpconnect)
			{
				if(self->eventhandlers.tcpconnect(
					&client,
					self->userdata
				))
				{
					SDLNet_TCP_AddSocket(self->socketset, client.socket);
					vec_pushback(&self->clients, client);
				}
				else /* API user don't want client to connect */
				{
					SDLNet_TCP_Close(client.socket);
				}
			}
			else /* default */
			{
				SDLNet_TCP_AddSocket(self->socketset, client.socket);
				vec_pushback(&self->clients, client);
			}
		}

		for(size_t i = 0; i < vec_getsize(&self->clients); i++)
		{
			if(SDLNet_SocketReady(self->clients[i].socket))
			{
				int recv = SDLNet_TCP_Recv(
					self->clients[i].socket, 
					self->buffer, 
					self->maxbufsize
				);
				if(recv <= 0) /* Client disconnected or error */
				{
					struct Client c = self->clients[i];
					SDLNet_TCP_DelSocket(self->socketset, c.socket);
					SDLNet_TCP_Close(c.socket);

					vec_collapse(&self->clients, i, 1);
					if(self->eventhandlers.tcpdisconnect)
					{
						self->eventhandlers.tcpdisconnect(
							&c, 
							(recv == 0) ? 
								SERVERDISCONNECT_USER : 
								SERVERDISCONNECT_ERROR,
							self->userdata
						);
					}
				}
				else /* Data received */
				{
					if(self->eventhandlers.tcpreceived)
					{
						if(!self->eventhandlers.tcpreceived(
							&self->clients[i],
							recv,
							self->buffer,
							self->userdata
						))
						{
							struct Client c = self->clients[i];
							SDLNet_TCP_DelSocket(self->socketset, c.socket);
							SDLNet_TCP_Close(c.socket);

							vec_collapse(&self->clients, i, 1);
							if(self->eventhandlers.tcpdisconnect)
							{
								self->eventhandlers.tcpdisconnect(
									&c, 
									SERVERDISCONNECT_KICKED,
									self->userdata
								);
							}
						}
					}
				}
			}
		}
	}

	if(self->type & SERVERTYPE_UDP)
	{
		int status = SDLNet_UDP_Recv(self->udp, &self->packet);
		if(status > 0)
		{
			if(self->eventhandlers.udpreceived)
			{
				self->eventhandlers.udpreceived(
					self->packet.address,
					self->packet.len,
					(char*)self->packet.data,
					self->userdata
				);
			}
		}
	}
}

void server_dtor(struct Server* self)
{
	assert(self);
	if(self->type & SERVERTYPE_UDP)
	{
		if(!self->buffer)
			free(self->packet.data);
		SDLNet_UDP_Close(self->udp);
	}

	if(self->type & SERVERTYPE_TCP)
	{
		for(size_t i = 0; i < vec_getsize(&self->clients); i++)
			SDLNet_TCP_Close(self->clients[i].socket);
		free(self->buffer);
		SDLNet_TCP_Close(self->tcp);
		SDLNet_FreeSocketSet(self->socketset);
	}

	vec_dtor(&self->clients);
	free(self);
}
