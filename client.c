#include "client.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

struct Client* client_ctor(struct Client* self, void* userdata)
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
			"version %i.%i.%i, but was linked with version %i.%i.%i",
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
	log_assert(self, "is NULL");
	log_assert(!(self->type & CLIENTTYPE_TCP), "is not TCP");
	log_assert(port < (1 << 16), "too large");
	log_assert(maxbufsize > 0, "invalid size");

	if(SDLNet_ResolveHost(&self->tcpaddress, ip, port))
		log_error(SDLNet_GetError());

	self->tcpsocket = SDLNet_TCP_Open(&self->tcpaddress);
	if(!self->tcpsocket)
	{
		log_warning(SDLNet_GetError());
		return 0;
	}

	self->tcpbuffer = malloc(maxbufsize);
	if(!self->tcpbuffer)
		log_error("malloc failed");
	self->tcpsocketset = SDLNet_AllocSocketSet(1);
	if(!self->tcpsocketset)
		log_error("SDLNet_AllocSocketSet failed");

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
	log_assert(self, "is NULL");
	log_assert(!(self->type & CLIENTTYPE_UDP), "is not UDP");
	log_assert(port < (1 << 16), "too large");
	log_assert(maxbufsize > 0, "invalid size");

	if(SDLNet_ResolveHost(&self->udpaddress, ip, port))
		log_error(SDLNet_GetError());
	
	self->udpsocket = SDLNet_UDP_Open(0);
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
	SDLNet_UDP_Bind(self->udpsocket, 0, &self->udpaddress);

	self->type |= CLIENTTYPE_UDP;
	self->udphandler = ondata;
	self->udpport = port;
	self->udpip = ip;
	return 1;
}

void client_sendtcp(struct Client* self, void* buffer, size_t bufsize)
{
	log_assert(self, "is NULL");
	log_assert(buffer, "is NULL");
	log_assert(bufsize, "is 0");
	log_assert(self->type & CLIENTTYPE_TCP, "is not TCP");
	log_assert((int)bufsize <= self->tcpmaxbufsize, "too large");

	int sent = SDLNet_TCP_Send(self->tcpsocket, buffer, bufsize);
	if(sent < (int)bufsize) /* not sent */
	{
		if(self->tcphandlers.tcpdisconnect)
		{
			self->tcphandlers.tcpdisconnect(
				CLIENTDISCONNECT_ERROR,
				self->userdata
			);
		}
		/* Temporary */
		log_error(SDLNet_GetError());
	}
}

void client_sendudp(struct Client* self, void* buffer, size_t bufsize)
{
	log_assert(self, "is NULL");
	log_assert(buffer, "is NULL");
	log_assert(bufsize, "is 0");
	log_assert(self->type & CLIENTTYPE_UDP, "is not UDP");
	log_assert((int)bufsize <= self->udppacket.maxlen, "too large");

	memmove(self->udppacket.data, buffer, bufsize);
	self->udppacket.len = bufsize;
	int sent = SDLNet_UDP_Send(
		self->udpsocket, 
		0, 
		&self->udppacket
	);

	if(!sent)
		log_warning("Failed to send udp data to destination");
}

void client_update(struct Client* self)
{
	log_assert(self, "is NULL");
	if(self->type & CLIENTTYPE_TCP)
	{
		SDLNet_CheckSockets(self->tcpsocketset, 0);
		if(SDLNet_SocketReady(self->tcpsocket))
		{
			int recv = SDLNet_TCP_Recv(
				self->tcpsocket,
				self->tcpbuffer, 
				self->tcpmaxbufsize
			);

			if(recv <= 0)
			{
				if(self->tcphandlers.tcpdisconnect)
				{
					self->tcphandlers.tcpdisconnect(
						CLIENTDISCONNECT_ERROR,
						self->userdata
					);
					log_warning("Lost connection to server");
					log_error(SDLNet_GetError());
				}
				else
				{;
					if(self->tcphandlers.tcpreceived)
					{
						self->tcphandlers.tcpreceived(
							recv,
							self->tcpbuffer,
							self->userdata
						);
					}
				}
			}
		}
	}

	if(self->type & CLIENTTYPE_UDP)
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

void client_dtor(struct Client* self)
{
	log_assert(self, "is NULL");
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
}

