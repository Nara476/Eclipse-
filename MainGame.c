#include "include/raylib.h"
#include <stdbool.h>
#include <stdio.h>

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
#define SPEED_INCREASE_RATE 0.2f
#define MAX_ENEMIES 100
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720
#define PROJECTILE_LIFETIME 2.0f  // Increased lifetime
#define COOLDOWN_TIME 2.0f 
#define GRAVITY 50.0f 
#define JUMP_FORCE 20.0f 
#define MAX_LIVES 10
#define AIZEN_PROJECTILE_COOLDOWN 5.0f

int main(void) 
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ALL AS PLANNED");
    InitAudioDevice();
    
    Music BgMusic = LoadMusicStream("BleachOst.mp3");
    Music BossMusic = LoadMusicStream("BossTheme.mp3");
    Sound Yokoso = LoadSound("Yokoso.mp3");
    SetMusicVolume(BgMusic,1.0f);
    PlayMusicStream(BgMusic);
    
    Rectangle Ground = {0, 600, SCREEN_WIDTH, SCREEN_HEIGHT / 3};
    
    Texture2D Getsuga = LoadTexture("GetsugaTensho.png");
    Texture2D Ichigo = LoadTexture("Ichigo.png");
    Texture2D EnemyTexture = LoadTexture("Enemy.png");
    Texture2D Aizen = LoadTexture("AizenBoss.png");
    Texture2D AizenProjectileTexture = LoadTexture("AizenProjectile.png"); // Load Aizen's projectile texture
    Texture2D BleachSky = LoadTexture("BleachSky.png");
    Texture2D GroundTexture = LoadTexture("Ground.png");

    bool bossMusicPlaying = false;

    Ichigo.width = 180;
    Ichigo.height = 150;
    EnemyTexture.width = Ichigo.width / 2;
    EnemyTexture.height = Ichigo.height / 2;
    float IchigoSpeed = 10;
    Vector2 IchigoPos = {50, Ground.y - Ichigo.height};
    bool IsFacingLeft = false;
    float IchigoVelocityY = 0.0f; 
    bool IsOnGround = true;
    int lives = MAX_LIVES;
    bool CanFlashStep = true;
    float FlashStepCooldown = 10.0f;
    float FlashStepTimer = 0;
    float FlashStepActiveTime = 0;
    float FlashStepCoolingDown = 0;

    Aizen.height = 200;
    Aizen.width = 300;
    Vector2 AizenPos = {600, Ground.y - Aizen.height};
    bool AizenActive = false;
    float AizenHealth = 20000.0f;
    float AizenAttackCooldown = 0.0f;
    
    bool AizenFinalBossActive = false;
    float AizenHealthBoss = 50000;
    
    Projectile aizenProjectile = {0}; 
     
    bool gameOver = false;  

    Projectile getsuga = {0};
    
    Enemy enemies[MAX_ENEMIES] = {0};
    int enemyCount = 0;
    float enemySpawnTimer = 0.0f;
    const float enemySpawnInterval = 3.0f;
    
    int score = 0;
    float cooldownTimer = 0.0f; 
    bool canShoot = true; 
    
    float gameTime = 0.0f;  
    float currentEnemySpeed = INITIAL_ENEMY_SPEED;


    SetTargetFPS(60);
    
    while (!WindowShouldClose()) 
    {
        
        UpdateMusicStream(BgMusic);
        UpdateMusicStream(BossMusic);
       

    
        float deltaTime = GetFrameTime();
        static float freezeTimer = 0.0f;
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
            if (freezeTimer > 0)
            {
                freezeTimer -= deltaTime;
            }
            else
            {
    
                if (IsKeyDown(KEY_D))
                {
                IsFacingLeft = false;
                IchigoPos.x += IchigoSpeed ;
                } 
                if (IchigoPos.x > SCREEN_WIDTH - Ichigo.width) 
                IchigoPos.x = SCREEN_WIDTH - Ichigo.width;
                if (IsKeyDown(KEY_A))
                { 
                    IchigoPos.x -= IchigoSpeed ;
                    IsFacingLeft = true;
                }
                if (IchigoPos.x < 0) 
                    IchigoPos.x = 0;
                if (IsKeyPressed(KEY_SPACE) && IsOnGround) 
                    {
                        IchigoVelocityY = -JUMP_FORCE;
                        IsOnGround = false;
                    }
    
            }
            IchigoVelocityY += GRAVITY * deltaTime;
            IchigoPos.y += IchigoVelocityY;

            if (IchigoPos.y >= Ground.y - Ichigo.height) 
            {
                IchigoPos.y = Ground.y - Ichigo.height;
                IchigoVelocityY = 0.0f;
                IsOnGround = true;
            }
                            
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && canShoot) 
            {
                getsuga.isActive = true;
                getsuga.isFacingLeft = IsFacingLeft;
                getsuga.position = (Vector2){IchigoPos.x + (IsFacingLeft ? -50 : 50), IchigoPos.y};
                getsuga.lifetime = 0.0f;

                canShoot = false;
                cooldownTimer = COOLDOWN_TIME;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CanFlashStep)
            {
                IchigoSpeed = 20;
                CanFlashStep = false;
                FlashStepCoolingDown = FlashStepCooldown;
            }
            if (FlashStepActiveTime >= 5.0f)
            {
               
                IchigoSpeed = 10;
                FlashStepActiveTime = 0;
            }
            if (!CanFlashStep)
            {
                FlashStepCoolingDown -= GetFrameTime();
                FlashStepActiveTime += GetFrameTime();
                FlashStepTimer += GetFrameTime();
                if (FlashStepTimer >= FlashStepCooldown)
                {
                   
                    CanFlashStep = true;
                    FlashStepTimer = 0;
                }
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
                            (Rectangle){getsuga.position.x-100, getsuga.position.y, 
                                       Getsuga.width/3 , Getsuga.height/2 },
                            (Rectangle){enemies[i].position.x, enemies[i].position.y, 
                                      enemies[i].texture.width, enemies[i].texture.height}
                        ))      
                    {
                            enemies[i].isActive = false;
                            score+= 10;
                    }
                }
            }
         
            gameTime += deltaTime;
            currentEnemySpeed = INITIAL_ENEMY_SPEED + (gameTime * SPEED_INCREASE_RATE);
            BeginDrawing();
            DrawTexture(BleachSky,-300, 0,WHITE);
            DrawRectangleRec(Ground, RED);
            DrawTextureV(GroundTexture,(Vector2){-10,-300},WHITE);
            
            ClearBackground(RAYWHITE);
        
            
            if ((score >= 50 && AizenHealth  > 0)) 
            {
                
                AizenActive = true;
                if (!bossMusicPlaying)  
                {   
                    PlaySound(Yokoso);
                    StopMusicStream(BgMusic);
                    PlayMusicStream(BossMusic);
                    SetMusicVolume(BossMusic, 0.5f);
                    bossMusicPlaying = true;  
                }   
            }
            

            if (AizenActive)
            {  
                

                float DamageCooldown = 2.0f;
                static bool MovingLeft = true;
                Rectangle AizenSrc = {0,0, ((MovingLeft)? -Aizen.width : Aizen.width),Aizen.height};
                Rectangle AizenDest = {AizenPos.x, AizenPos.y + 40, Aizen.width, Aizen.height - 40};
                Vector2 origin = {0,0};
                DrawTexturePro(Aizen,AizenSrc,AizenDest,origin,0,WHITE);
                DrawText(TextFormat("Health %f",AizenHealth),AizenPos.x,AizenPos.y -50,40,BLACK);
                    static float DamageTimer = 0.0f;
                    if (MovingLeft)
                    {
                        AizenPos.x -= 2;
                        if (AizenPos.x <= 0)
                        MovingLeft = false;
                    }
                    else
                    {
                        AizenPos.x += 2;
                        if (AizenPos.x >= SCREEN_WIDTH - Aizen.width)
                        MovingLeft = true;
                    }
                    if (DamageTimer > 0)
                        DamageTimer -= deltaTime;
                    if (DamageTimer <= 0 && CheckCollisionRecs(
                         (Rectangle){IchigoPos.x, IchigoPos.y, Ichigo.width, Ichigo.height},
                        (Rectangle){AizenPos.x, AizenPos.y + 40, Aizen.width - 100, Aizen.height - 100}))
                    {
                        lives--;
                        DamageTimer = DamageCooldown;
                        if (lives <= 0)
                        gameOver = true;

                        
                    }
                     if (getsuga.isActive && CheckCollisionRecs(
                        (Rectangle){getsuga.position.x, getsuga.position.y, Getsuga.width, Getsuga.height},
                        (Rectangle){AizenPos.x, AizenPos.y + 40, Aizen.width, Aizen.height - 40}))
                    {
                        AizenHealth -= 10;
                        if (AizenHealth <= 0)
                        {
                            AizenActive = false;
                            StopMusicStream(BossMusic);
                            PlayMusicStream(BgMusic);
                           
                        }
                        
                    }


                    // Aizen's projectile attack logic
                    if (AizenAttackCooldown <= 0.0f)
                    {
                        aizenProjectile.isActive = true;
                        aizenProjectile.position = (Vector2){AizenPos.x + (MovingLeft ? -Aizen.width / 2 : Aizen.width / 2), AizenPos.y + Aizen.height / 2};
                        aizenProjectile.isFacingLeft = MovingLeft;
                        aizenProjectile.lifetime = 0.0f;
                        AizenAttackCooldown = AIZEN_PROJECTILE_COOLDOWN;
                    }
                    else
                    {
                        AizenAttackCooldown -= deltaTime;
                    }

                    if (aizenProjectile.isActive)
                    {
                        aizenProjectile.position.x += aizenProjectile.isFacingLeft ? -6.0f : 6.0f; // Slower speed
                        aizenProjectile.lifetime += deltaTime;

                        // Deactivate the projectile only if it goes off-screen or exceeds its lifetime
                        if (aizenProjectile.position.x < 0 || aizenProjectile.position.x > SCREEN_WIDTH || 
                            aizenProjectile.lifetime >= PROJECTILE_LIFETIME)
                        {
                            aizenProjectile.isActive = false;
                        }

                       if (CheckCollisionRecs(
                        (Rectangle){IchigoPos.x, IchigoPos.y, Ichigo.width, Ichigo.height},
                        (Rectangle){aizenProjectile.position.x, aizenProjectile.position.y, 
                                               AizenProjectileTexture.width - 150, AizenProjectileTexture.height - 150}))
                        {
                            if (aizenProjectile.isActive) 
                            { 
       
                                freezeTimer = 1.0f; // Freeze Ichigo for 2 seconds
                                aizenProjectile.lifetime = 0.0f; // Reset projectile's lifetime instead of deactivating
                            }
                            if (lives <= 0)
                            gameOver = true;
                        }
                    }

                    if (aizenProjectile.isActive)
                    {
                        DrawTexturePro(AizenProjectileTexture, 
                                     (Rectangle){0, 0, (MovingLeft? -(AizenProjectileTexture.width) : (AizenProjectileTexture.width)), AizenProjectileTexture.height - 50}, 
                                     (Rectangle){aizenProjectile.position.x, aizenProjectile.position.y, 
                                               AizenProjectileTexture.width -150, AizenProjectileTexture.height - 150}, 
                                     (Vector2){0, 0}, 0.0f, WHITE);
                    }
            }

            if ((score >= 1000 && AizenHealthBoss  > 0)) 
            {
                AizenFinalBossActive = true;
                if (!bossMusicPlaying) 
                {
                    PlaySound(Yokoso);
                    StopMusicStream(BgMusic);
                    PlayMusicStream(BossMusic);
                    SetMusicVolume(BossMusic, 0.5f);
                    bossMusicPlaying = true; 
                }
               
            }

            if (AizenFinalBossActive)
            {
                float DamageCooldown = 2.0f;
                static bool MovingLeft = true;
                Rectangle AizenSrc = {0,0, ((MovingLeft)? -Aizen.width : Aizen.width),Aizen.height};
                Rectangle AizenDest = {AizenPos.x, AizenPos.y, Aizen.width, Aizen.height};
                Vector2 origin = {0,0};
                DrawTexturePro(Aizen,AizenSrc,AizenDest,origin,0,WHITE);
                DrawText(TextFormat("Health %f",AizenHealthBoss),AizenPos.x,AizenPos.y -50,40,BLACK);
                    static float DamageTimer = 0.0f;
                    if (MovingLeft)
                    {
                        AizenPos.x -= 1;
                        if (AizenPos.x <= 0)
                        MovingLeft = false;
                    }
                    else
                    {
                        AizenPos.x += 1;
                        if (AizenPos.x >= SCREEN_WIDTH - Aizen.width)
                        MovingLeft = true;
                    }
                    if (DamageTimer > 0)
                        DamageTimer -= deltaTime;
                    if (DamageTimer <= 0 && CheckCollisionRecs(
                         (Rectangle){IchigoPos.x, IchigoPos.y, Ichigo.width, Ichigo.height},
                        (Rectangle){AizenPos.x, AizenPos.y, Aizen.width, Aizen.height}))
                    {
                        lives-= 3;
                        DamageTimer = DamageCooldown;
                        if (lives <= 0)
                        gameOver = true;

                        
                    }
                     if (getsuga.isActive && CheckCollisionRecs(
                        (Rectangle){getsuga.position.x, getsuga.position.y, Getsuga.width, Getsuga.height},
                        (Rectangle){AizenPos.x, AizenPos.y, Aizen.width, Aizen.height}))
                    {
                        AizenHealthBoss -= 10;
                        if (AizenHealthBoss <= 0)
                        {
                            AizenFinalBossActive = false;
                            StopMusicStream(BossMusic);
                            PlayMusicStream(BgMusic);
                           
                        }
                        
                    }


                    // Aizen's projectile attack logic
                    if (AizenAttackCooldown <= 0.0f)
                    {
                        aizenProjectile.isActive = true;
                        aizenProjectile.position = (Vector2){AizenPos.x + (MovingLeft ? -Aizen.width / 2 : Aizen.width / 2), AizenPos.y + Aizen.height / 2};
                        aizenProjectile.isFacingLeft = MovingLeft;
                        aizenProjectile.lifetime = 0.0f;
                        AizenAttackCooldown = AIZEN_PROJECTILE_COOLDOWN;
                    }
                    else
                    {
                        AizenAttackCooldown -= deltaTime;
                    }

                    if (aizenProjectile.isActive)
                    {
                        aizenProjectile.position.x += aizenProjectile.isFacingLeft ? -5.0f : 5.0f; // Slower speed
                        aizenProjectile.lifetime += deltaTime;

                        // Deactivate the projectile only if it goes off-screen or exceeds its lifetime
                        if (aizenProjectile.position.x < 0 || aizenProjectile.position.x > SCREEN_WIDTH || 
                            aizenProjectile.lifetime >= PROJECTILE_LIFETIME)
                        {
                            aizenProjectile.isActive = false;
                        }

                        if (CheckCollisionRecs(
                        (Rectangle){IchigoPos.x, IchigoPos.y, Ichigo.width, Ichigo.height},
                        (Rectangle){aizenProjectile.position.x, aizenProjectile.position.y, 
                                               AizenProjectileTexture.width, AizenProjectileTexture.height - 150}))
                        {
                            if (aizenProjectile.isActive) 
                            { 
       
                                freezeTimer = 1.0f; // Freeze Ichigo for 2 seconds
                                aizenProjectile.lifetime = 0.0f; // Reset projectile's lifetime instead of deactivating
                            }
                            if (lives <= 0)
                            gameOver = true;
                        }
                    }

                    if (aizenProjectile.isActive)
                    {
                        DrawTexturePro(AizenProjectileTexture, 
                                     (Rectangle){0, 0, (MovingLeft? -(AizenProjectileTexture.width) : (AizenProjectileTexture.width)), AizenProjectileTexture.height - 50}, 
                                     (Rectangle){aizenProjectile.position.x, aizenProjectile.position.y, 
                                               AizenProjectileTexture.width, AizenProjectileTexture.height - 150}, 
                                     (Vector2){0, 0}, 0.0f, WHITE);
                    } 
            }
            
            
           
            
            
            Rectangle start = {0, 0, (IsFacingLeft ? -Ichigo.width : Ichigo.width), Ichigo.height};
            Rectangle Dest = {IchigoPos.x, IchigoPos.y+10, Ichigo.width, Ichigo.height};
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
                             (Rectangle){getsuga.position.x-100, getsuga.position.y, 
                                       Getsuga.width/3 , Getsuga.height/2 }, 
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
            if (!CanFlashStep)
            {
                DrawText(TextFormat(" FlashStep Cooldown: %.1f", FlashStepCoolingDown), 
                        SCREEN_WIDTH / 2 - 70, 70, 30, BLACK);
            } else 
            {
                DrawText("Ready to Flash Step!", SCREEN_WIDTH / 2 - 50, 70, 30, BLACK);
            }

            DrawText("Press M1 to Getsuga Tensho", 10, 10, 30, BLACK);
            DrawText("Press SPACE to Jump", 10, 130, 30, BLACK);
            DrawText("Press M2 to Flash Step", 10, 50, 30, BLACK);
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
            DrawText(scoreText, scorePos.x - 30, scorePos.y, 40, BLACK);
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
                AizenActive = false;
                AizenHealth = 2000.0f;
                AizenAttackCooldown = 0.0f;
                aizenProjectile.isActive = false;
                AizenFinalBossActive = false;
                currentEnemySpeed = INITIAL_ENEMY_SPEED;
                gameTime = 0.0f;
                StopMusicStream(BossMusic);
                bossMusicPlaying = false;
                
            }
        }

        
        EndDrawing();
    }
    
    UnloadMusicStream(BgMusic);
    UnloadMusicStream(BossMusic);
    UnloadTexture(Getsuga);
    UnloadTexture(Ichigo);
    UnloadTexture(EnemyTexture);
    CloseWindow();
    return 0;
}