debug:
	gcc ../../src/*.c -I../../ -ggdb3 -Og -Wall -Wextra -Wshadow -shared -I ./include -L. -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer -lglew32 -o libAPI.dll

release:
	gcc ../../src/*.c -I../../ -Wall -Wextra -Wshadow -march=native -O2 -shared -I./include -L. -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lglew32 -o libAPI.dll
