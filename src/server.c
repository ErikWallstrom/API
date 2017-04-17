#include "server.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

struct Server* server_ctor(struct Server* self, void* userdata)
{
	log_assert(self, "is NULL");

	const SDL_version *link_version = SDLNet_Linked_Version();
	SDL_version compile_version;
	SDL_NET_VERSION(&compile_version);
	if(compile_version.major != link_version->major ||
		compile_version.minor != link_version->minor ||
		compile_version.patch != link_version->patch)
	{
		log_warning(
			"Program was compiled with SDL_image "
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
		log_error(SDLNet_GetError());
	
	*self = (struct Server){0};
	self->userdata = userdata;
	return self;
}

int server_inittcp(
	struct Server* self, 
	int port,
	int maxbufsize,
	int maxclients,
	struct ServerTCPEventHandlers handlers
)
{
	log_assert(self, "is NULL");
	log_assert(!(self->type & SERVERTYPE_TCP), "is not TCP");
	log_assert(port < (1 << 16), "too large");
	log_assert(maxbufsize > 0, "too small");
	log_assert(maxclients > 0, "too small");

	if(SDLNet_ResolveHost(&self->tcpaddress, NULL, port))
		log_error(SDLNet_GetError());

	self->tcpsocket = SDLNet_TCP_Open(&self->tcpaddress);
	if(!self->tcpsocket)
	{
		//debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);
		log_warning(SDLNet_GetError());
		return 0;
	}

	self->tcpbuffer = malloc(maxbufsize);
	if(!self->tcpbuffer)
		log_error("malloc failed");
	self->tcpsocketset = SDLNet_AllocSocketSet(maxclients + 1);
	if(!self->tcpsocketset)
		log_error("SDLNet_AllocSocketSet failed");

	SDLNet_TCP_AddSocket(self->tcpsocketset, self->tcpsocket);
	self->tcpclients = vec_ctor(struct ServerTCPClient, maxclients);

	self->type |= SERVERTYPE_TCP;
	self->tcpport = port;
	self->tcphandlers = handlers;
	self->tcpmaxbufsize = maxbufsize;
	self->tcpmaxclients = maxclients;
	return 1;
}

int server_initudp(
	struct Server* self, 
	int port, 
	int maxbufsize,
	ServerOnUDPData ondata
)
{
	log_assert(self, "is NULL");
	log_assert(!(self->type & SERVERTYPE_UDP), "is not UDP");
	log_assert(port < (1 << 16), "too large");
	log_assert(maxbufsize > 0, "too small");

	self->udpsocket = SDLNet_UDP_Open(port);
	if(!self->udpsocket)
	{
		//debug(SDLNet_GetError(), ERRORTYPE_APPLICATION);
		log_warning(SDLNet_GetError());
		return 0;
	}
	self->udppacket.data = malloc(maxbufsize);
	if(!self->udppacket.data)
		log_error("malloc failed");

	self->udppacket.maxlen = maxbufsize;
	self->udppacket.len = maxbufsize;

	self->type |= SERVERTYPE_UDP;
	self->udphandler = ondata;
	self->udpport = port;
	return 1;
}

void server_sendtcp(
	struct Server* self, 
	struct ServerTCPClient* dest, 
	void* buffer, 
	size_t bufsize
)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");
	log_assert(buffer, "is NULL");
	log_assert(bufsize, "is 0");
	log_assert(self->type & SERVERTYPE_TCP, "is not TCP");
	log_assert((int)bufsize <= self->tcpmaxbufsize, "too large");

	int sent = SDLNet_TCP_Send(dest->socket, buffer, bufsize);
	if(sent < (int)bufsize) /* not sent */
	{
		int index = -1;
		for(size_t i = 0; i < vec_getsize(self->tcpclients); i++)
		{
			if(self->tcpclients[i].socket == dest->socket)
			{
				index = (int)i;
				break;
			}
		}

		log_assert(index != -1, "client not found");

		SDLNet_TCP_DelSocket(self->tcpsocketset, dest->socket);
		SDLNet_TCP_Close(dest->socket);

		vec_collapse(self->tcpclients, index, 1);
		if(self->tcphandlers.tcpdisconnect)
		{
			self->tcphandlers.tcpdisconnect(
				dest, 
				SERVERDISCONNECT_USER, /* could be error though */
				self->userdata
			);
		}
	}
}

void server_sendudp(
	struct Server* self, 
	IPaddress dest, 
	void* buffer, 
	size_t bufsize
)
{
	log_assert(self, "is NULL");
	log_assert(buffer, "is NULL");
	log_assert(bufsize, "is 0");
	log_assert(self->type & SERVERTYPE_UDP, "is not UDP");
	log_assert((int)bufsize <= self->udppacket.maxlen, "too large");

	memmove(self->udppacket.data, buffer, bufsize);
	self->udppacket.len = bufsize;
	self->udppacket.address = dest;
	int sent = SDLNet_UDP_Send(
		self->udpsocket, 
		-1, 
		&self->udppacket
	);

	if(!sent)
		log_warning("Failed to send udp data to destination");
}

void server_update(struct Server* self)
{
	log_assert(self, "is NULL");
	if(self->type & SERVERTYPE_TCP)
	{
		SDLNet_CheckSockets(self->tcpsocketset, 0);
		TCPsocket newclient = SDLNet_TCP_Accept(self->tcpsocket);
		if(newclient)
		{
			struct ServerTCPClient client = (struct ServerTCPClient){
				.socket = newclient,
				.address = *SDLNet_TCP_GetPeerAddress(newclient)
			};
			if(self->tcphandlers.tcpconnect)
			{
				if(self->tcphandlers.tcpconnect(
					&client,
					((int)vec_getsize(self->tcpclients) >= 
					 	self->tcpmaxclients) ? 1 : 0,
					self->userdata
				)) /* API user wants client to connect */
				{
					SDLNet_TCP_AddSocket(self->tcpsocketset, client.socket);
					vec_pushback(self->tcpclients, client);
				}
				else /* API user don't want client to connect */
				{
					SDLNet_TCP_Close(client.socket);
				}
			}
			else /* default behavior */
			{
				SDLNet_TCP_AddSocket(self->tcpsocketset, client.socket);
				vec_pushback(self->tcpclients, client);
			}
		}

		for(size_t i = 0; i < vec_getsize(self->tcpclients); i++)
		{
			if(SDLNet_SocketReady(self->tcpclients[i].socket))
			{
				int recv = SDLNet_TCP_Recv(
					self->tcpclients[i].socket, 
					self->tcpbuffer, 
					self->tcpmaxbufsize
				);
				if(recv <= 0) /* ServerClient disconnected or error */
				{
					struct ServerTCPClient c = self->tcpclients[i];
					SDLNet_TCP_DelSocket(self->tcpsocketset, c.socket);
					SDLNet_TCP_Close(c.socket);

					vec_collapse(self->tcpclients, i, 1);
					if(self->tcphandlers.tcpdisconnect)
					{
						self->tcphandlers.tcpdisconnect(
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
					if(self->tcphandlers.tcpreceived)
					{
						if(!self->tcphandlers.tcpreceived(
							&self->tcpclients[i],
							recv,
							self->tcpbuffer,
							self->userdata
						))
						{
							struct ServerTCPClient c = self->tcpclients[i];
							SDLNet_TCP_DelSocket(self->tcpsocketset, c.socket);
							SDLNet_TCP_Close(c.socket);

							vec_collapse(self->tcpclients, i, 1);
							if(self->tcphandlers.tcpdisconnect)
							{
								self->tcphandlers.tcpdisconnect(
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
		int status = SDLNet_UDP_Recv(self->udpsocket, &self->udppacket);
		if(status > 0)
		{
			if(self->udphandler)
			{
				self->udphandler(
					self->udppacket.address,
					self->udppacket.len,
					(char*)self->udppacket.data,
					self->userdata
				);
			}
		}
		else if(status == -1)
			log_warning("Something UDP related failed...");
	}
}

void server_dtor(struct Server* self)
{
	log_assert(self, "is NULL");
	if(self->type & SERVERTYPE_UDP)
	{
		free(self->udppacket.data);
		SDLNet_UDP_Close(self->udpsocket);
	}

	if(self->type & SERVERTYPE_TCP)
	{
		for(size_t i = 0; i < vec_getsize(self->tcpclients); i++)
			SDLNet_TCP_Close(self->tcpclients[i].socket);

		free(self->tcpbuffer);
		vec_dtor(self->tcpclients);
		SDLNet_TCP_Close(self->tcpsocket);
		SDLNet_FreeSocketSet(self->tcpsocketset);
	}
}

