#include "include/raylib.h"
#include <stdbool.h>
typedef struct {
    Vector2 position;
    bool isActive;
    bool isFacingLeft;
    float lifetime; 
} Projectile;
typedef struct {
    Vector2 position;
    Texture2D texture;
    bool isActive;
    bool movingRight;  
    float speed;      
} Enemy;

#define LEFT_SIDE 0
#define RIGHT_SIDE 1
#define INITIAL_ENEMY_SPEED 3.0f
#define SPEED_INCREASE_RATE 0.5f
#define MAX_ENEMIES 100
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720
#define PROJECTILE_LIFETIME 0.5f 
#define COOLDOWN_TIME 2.0f 
#define GRAVITY 50.0f 
#define JUMP_FORCE 20.0f 
#define MAX_LIVES 3

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ALL AS PLANNED");
    InitAudioDevice();
    
    Music BgMusic = LoadMusicStream("La Espada.mp3");
    PlayMusicStream(BgMusic);
    
    Rectangle Ground = {0, 600, SCREEN_WIDTH, SCREEN_HEIGHT / 3};
    
    Texture2D Getsuga = LoadTexture("GetsugaTensho.png");
    Texture2D Ichigo = LoadTexture("Ichigo.png");
    Texture2D EnemyTexture = LoadTexture("Enemy.png");

    Ichigo.width = 100;
    Ichigo.height = 150;
    EnemyTexture.width = Ichigo.width / 2;
    EnemyTexture.height = Ichigo.height / 2;
    
    float IchigoSpeed = 5;
    Vector2 IchigoPos = {50, Ground.y - Ichigo.height};
    bool IsFacingLeft = false;
    float IchigoVelocityY = 0.0f; 
    bool IsOnGround = true;
    int lives = MAX_LIVES;  
    bool gameOver = false;  

    Projectile getsuga = {0};
    
    Enemy enemies[MAX_ENEMIES] = {0};
    int enemyCount = 0;
    float enemySpawnTimer = 0.0f;
    const float enemySpawnInterval = 2.0f;
    
    int score = 0;
    float cooldownTimer = 0.0f; 
    bool canShoot = true; 
    
    float gameTime = 0.0f;  
    float currentEnemySpeed = INITIAL_ENEMY_SPEED;

    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        UpdateMusicStream(BgMusic);
        float deltaTime = GetFrameTime();

        if (!gameOver) 
        {
            if (!canShoot) 
            {
                cooldownTimer -= deltaTime;
                if (cooldownTimer <= 0.0f) 
                    canShoot = true;
            }

            
            enemySpawnTimer += deltaTime;
            if (enemySpawnTimer >= enemySpawnInterval && enemyCount < MAX_ENEMIES) 
            {
                int spawnSide = GetRandomValue(LEFT_SIDE, RIGHT_SIDE);
    
                if (spawnSide == LEFT_SIDE) 
                {
                    enemies[enemyCount].position = (Vector2){-EnemyTexture.width, 
                                               Ground.y - EnemyTexture.height};
                    enemies[enemyCount].movingRight = true;
                } 
                else 
                {
                    enemies[enemyCount].position = (Vector2){SCREEN_WIDTH, 
                                               Ground.y - EnemyTexture.height};
                    enemies[enemyCount].movingRight = false;
                }
    
                enemies[enemyCount].texture = EnemyTexture;
                enemies[enemyCount].isActive = true;
                enemies[enemyCount].speed = currentEnemySpeed;  
                enemyCount++;
                enemySpawnTimer = 0.0f;
            }
            
            for (int i = 0; i < enemyCount; i++) 
            {
                if (enemies[i].isActive) 
                {
    
                    if (enemies[i].movingRight) 
                    {
                        enemies[i].position.x += enemies[i].speed;
                        if (enemies[i].position.x > SCREEN_WIDTH) 
                            enemies[i].isActive = false;
                    } 
                    else 
                    {
                        enemies[i].position.x -= enemies[i].speed;
                        if (enemies[i].position.x + enemies[i].texture.width < 0) 
                        enemies[i].isActive = false;
                    }
                    
                     if (CheckCollisionRecs(
                        (Rectangle){IchigoPos.x, IchigoPos.y, Ichigo.width, Ichigo.height},
                        (Rectangle){enemies[i].position.x, enemies[i].position.y, 
                        enemies[i].texture.width, enemies[i].texture.height})) 
                        {
                            enemies[i].isActive = false;
                            lives--;
                            if (lives <= 0)
                            gameOver = true;
                        }
                }
            }
            
            
            if (IsKeyDown(KEY_A)) 
            {
                IsFacingLeft = true;
                IchigoPos.x -= IchigoSpeed;
    
                if (IchigoPos.x < 0) 
                    IchigoPos.x = 0;
            }
            if (IsKeyDown(KEY_D)) 
            {
                IsFacingLeft = false;
                IchigoPos.x += IchigoSpeed;
    
                if (IchigoPos.x > SCREEN_WIDTH - Ichigo.width) 
                IchigoPos.x = SCREEN_WIDTH - Ichigo.width;
            }
            if (IsKeyPressed(KEY_SPACE) && IsOnGround) 
            {
                IchigoVelocityY = -JUMP_FORCE;
                IsOnGround = false;
            }

            IchigoVelocityY += GRAVITY * deltaTime;
            IchigoPos.y += IchigoVelocityY;

            if (IchigoPos.y >= Ground.y - Ichigo.height) 
            {
                IchigoPos.y = Ground.y - Ichigo.height;
                IchigoVelocityY = 0.0f;
                IsOnGround = true;
            }
            
            if (IsKeyPressed(KEY_E) && canShoot) 
            {
                getsuga.isActive = true;
                getsuga.isFacingLeft = IsFacingLeft;
                getsuga.position = (Vector2){IchigoPos.x + (IsFacingLeft ? -50 : 50), IchigoPos.y};
                getsuga.lifetime = 0.0f;

                canShoot = false;
                cooldownTimer = COOLDOWN_TIME;
            }
        
            if (getsuga.isActive) 
            {
                getsuga.position.x += getsuga.isFacingLeft ? -10.0f : 10.0f;
                getsuga.lifetime += deltaTime;
                
                if (getsuga.position.x < 0 || getsuga.position.x > SCREEN_WIDTH || 
                    getsuga.lifetime >= PROJECTILE_LIFETIME) 
                         getsuga.isActive = false;
                
                for (int i = 0; i < enemyCount; i++) 
                {
                    if (enemies[i].isActive && 
                        CheckCollisionRecs(
                            (Rectangle){getsuga.position.x, getsuga.position.y, 
                                      Getsuga.width / 2, Getsuga.height},
                            (Rectangle){enemies[i].position.x, enemies[i].position.y, 
                                      enemies[i].texture.width, enemies[i].texture.height}
                        ))      
                    {
                            enemies[i].isActive = false;
                            score++;
                    }
                }
            }
        }
        if (!gameOver) 
        {
            gameTime += deltaTime;
            currentEnemySpeed = INITIAL_ENEMY_SPEED + (gameTime * SPEED_INCREASE_RATE);
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        if (!gameOver) 
        {
            DrawRectangleRec(Ground, RED);
            
            Rectangle start = {0, 0, (IsFacingLeft ? -Ichigo.width : Ichigo.width), Ichigo.height};
            Rectangle Dest = {IchigoPos.x, IchigoPos.y, Ichigo.width, Ichigo.height};
            DrawTexturePro(Ichigo, start, Dest, (Vector2){0, 0}, 0.0f, WHITE);
            
            for (int i = 0; i < enemyCount; i++) 
            {
                if (enemies[i].isActive) 
                {
                    Rectangle sourceRec = {0, 0, enemies[i].movingRight ? enemies[i].texture.width : -enemies[i].texture.width,
                    enemies[i].texture.height};
                    Rectangle destRec = 
                                        {
                                            enemies[i].position.x,
                                            enemies[i].position.y,
                                            enemies[i].texture.width,
                                            enemies[i].texture.height
                                        };
                    DrawTexturePro(enemies[i].texture, sourceRec, destRec, 
                    (Vector2){0, 0}, 0.0f, WHITE);
                }
            }

            if (getsuga.isActive) 
            {
                DrawTexturePro(Getsuga, 
                             (Rectangle){0, 0, Getsuga.width, Getsuga.height}, 
                             (Rectangle){getsuga.position.x, getsuga.position.y, 
                                       Getsuga.width / 3, Getsuga.height / 2}, 
                             (Vector2){0, 0}, 0.0f, WHITE);
            }
            
            DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH - 150, 10, 30, BLACK);
            DrawText(TextFormat("Lives: %d", lives), 10, 90, 30, BLACK);

            if (!canShoot) 
            {
                DrawText(TextFormat("Cooldown: %.1f", cooldownTimer), 
                        SCREEN_WIDTH / 2 - 70, 10, 30, BLACK);
            } else 
            {
                DrawText("Ready to Shoot!", SCREEN_WIDTH / 2 - 50, 10, 30, BLACK);
            }

            DrawText("Press E to Getsuga Tensho", 10, 10, 30, BLACK);
            DrawText("Press SPACE to Jump", 10, 50, 30, BLACK);
        } 
        else 
        {
            const char* gameOverText = "GAME OVER";
            const char* scoreText = TextFormat("Final Score: %d", score);
            const char* restartText = "Press R to Restart";
            
            Vector2 gameOverPos = 
            {
                (SCREEN_WIDTH - MeasureText(gameOverText, 60)) / 2,
                SCREEN_HEIGHT / 2 - 60
            };
            Vector2 scorePos = 
            {
                (SCREEN_WIDTH - MeasureText(scoreText, 40)) / 2,
                SCREEN_HEIGHT / 2
            };
            Vector2 restartPos = 
            {
                (SCREEN_WIDTH - MeasureText(restartText, 30)) / 2,
                SCREEN_HEIGHT / 2 + 30
            };
            
            DrawText(gameOverText, gameOverPos.x, gameOverPos.y, 60, RED);
            DrawText(scoreText, scorePos.x, scorePos.y, 40, BLACK);
            DrawText(restartText, restartPos.x, restartPos.y, 30, DARKGRAY);
            
            if (IsKeyPressed(KEY_R)) 
            {
                lives = MAX_LIVES;
                score = 0;
                gameOver = false;
                IchigoPos = (Vector2){50, Ground.y - Ichigo.height};
                enemyCount = 0;
                enemySpawnTimer = 0.0f;
                getsuga.isActive = false;
                canShoot = true;
                cooldownTimer = 0.0f;
            }
        }
        
        EndDrawing();
    }
    
    UnloadMusicStream(BgMusic);
    UnloadTexture(Getsuga);
    UnloadTexture(Ichigo);
    UnloadTexture(EnemyTexture);
    CloseWindow();
    return 0;
}