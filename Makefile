build:
	gcc ./game.c ./window.c ./error.c ./imageloader.c ./vec.c ./server.c ./fontloader.c ./texturehandler.c -Wall -Wextra -Wshadow -shared -fPIC -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -o libAPI.so 
