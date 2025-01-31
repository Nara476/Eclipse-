#include "include/raylib.h"
#include <string.h> 

int main()
{

    InitWindow(1000, 800, "Blue");
    InitAudioDevice();

    Music Blue = LoadMusicStream("Blue.mp3");
    const char* lyrics[] = {
        "Blue Yung Kai",
        "Your morning eyes, I could stare like watching stars",  
        "I could walk you by, and I'll tell without a thought",  
        "You'd be mine, would you mind if I took your hand tonight?", 
        "Know you're all that I want this life" ,

        "I'll imagine we fell in love",
        "I'll nap under moonlight skies with you" ,
        "I think I'll picture us, you with the waves",
        "The ocean's colors on your face",
        "I'll leave my heart with your air",
        "So let me fly with you",
        "Will you be forever with me?"
    };
    float lyricTimings[] = { 0.0f,19.0f, 26.5f, 32.5f, 40.0f,47.5f,50.5f,54.5f,58.0f,61.5f,65.5f,69.5f,75.0f }; // Timings for each lyric line
    int currentLyricIndex = 0;


    int currentLetterIndex = 0;
    float letterDisplayTimer = 0.0f;
    const float letterDisplayInterval = 0.1f; 

    PlayMusicStream(Blue);

    while (!WindowShouldClose())
    {
        
        UpdateMusicStream(Blue);
        float currentTime = GetMusicTimePlayed(Blue);

        
        if (currentLyricIndex < sizeof(lyricTimings) / sizeof(lyricTimings[0]) &&
            currentTime >= lyricTimings[currentLyricIndex])
        {
        
            currentLetterIndex = 0;
            letterDisplayTimer = 0.0f;
            currentLyricIndex++;
        }

        
        letterDisplayTimer += GetFrameTime();


        if (currentLyricIndex > 0 && currentLetterIndex < strlen(lyrics[currentLyricIndex - 1]))
        {
            if (letterDisplayTimer >= letterDisplayInterval)
            {
                currentLetterIndex++;
                letterDisplayTimer = 0.0f; 
            }
        }

        
        ClearBackground(BLACK);

        
        BeginDrawing();


        if (currentLyricIndex > 0)
        {
            char displayedLyric[256] = {0};
            strncpy(displayedLyric, lyrics[currentLyricIndex - 1], currentLetterIndex);
            DrawText(displayedLyric, 50, 400, 30, BLUE);
        }

      
        EndDrawing();
    }

    
    UnloadMusicStream(Blue);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}