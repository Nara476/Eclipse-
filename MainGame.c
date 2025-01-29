#include "include/raylib.h"
int main(void) 
    {
    const int screenWidth = 1080;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "ALL AS PLANNED");

    InitAudioDevice();
    Music BgMusic = LoadMusicStream("La Espada.mp3");
    PlayMusicStream(BgMusic);


    Rectangle Ground = {0,600,screenWidth,screenHeight/3};


    Texture2D Ichigo = LoadTexture("Ichigo.png");
    Ichigo.height -= 600;
    Ichigo.width -= 600;
    float IchigoSpeed = 5;
    Vector2 IchigoPos = {0,460};
    bool IsFacingLeft = false;

    SetTargetFPS(60);


    while (!WindowShouldClose()) {
        UpdateMusicStream(BgMusic);
        BeginDrawing();
            if (IsKeyDown(KEY_A))
                {
                    IsFacingLeft = true;
                    IchigoPos.x -= IchigoSpeed;
                }
            if (IsKeyDown(KEY_D))
            {
                IsFacingLeft = false;
                IchigoPos.x += IchigoSpeed;
            }
                
            ClearBackground(RAYWHITE);
            DrawRectangleRec(Ground ,RED);

            Rectangle start = {0,0,((IsFacingLeft)? -Ichigo.width : Ichigo.width),Ichigo.height};
            Rectangle Dest = {IchigoPos.x,IchigoPos.y , Ichigo.width, Ichigo.height};
            Vector2 origin = { 0, 0 };
            
            DrawTexturePro(Ichigo, start, Dest, origin, 0.0f, WHITE);
            DrawText("Press E to GentsugaTensho", 0,0,30,BLACK);

        EndDrawing();
    }
    UnloadMusicStream(BgMusic);
    CloseWindow();

    return 0;
}