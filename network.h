#ifndef NETWORK_H
#define NETWORK_H

enum MessageType
{
	/* Server messages */
	MESSAGET_DISCONNECT,
	MESSAGET_ACCEPT,
	MESSAGET_QUIT,
	MESSAGET_FULL,

	/* Client messages */
	MESSAGE_KEYDOWN,
	MESSAGE_KEYUP
};

struct Message
{
	enum MessageType type;
	char buffer[6];
};

struct GameState
{
	struct {float x; float y;} players[2];
};

#endif
