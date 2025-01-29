all:
	gcc MainGame.c -o MainGame.exe -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm