all:
gcc Blue.c -o Blue.exe -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
gcc MainGame.c -o MainGame.exe -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

	