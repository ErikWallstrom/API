debug:
	gcc ../*.c -ggdb3 -Og -Wall -Wextra -Wshadow -shared -I ./include -I .. -L. -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer -o libAPI.dll

release:
	gcc ../*.c -Wall -Wextra -Wshadow -march=native -O2 -shared -I./include -L. -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -o libAPI.dll
