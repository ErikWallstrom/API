build:
	gcc ./game.c ./window.c ./error.c ./imagehandler.c ./vec.c -Wall -Wextra -Wshadow -shared -fPIC -lSDL2 -lSDL2_image -o libAPI.so 
