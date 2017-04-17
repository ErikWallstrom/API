debug:
	gcc ../../src/*.c -I../../ -ggdb3 -Og -Wall -Wextra -Wshadow -shared -fPIC -lGLEW -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer -o libAPI.so

release:
	gcc ../../src/*.c -I../../ -Wall -Wextra -Wshadow -march=native -O2 -shared -fPIC -lGLEW -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -o libAPI.so 
