#include "raylib.h"
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include "raymath.h" // Required for Lerp

enum BallType {
    GREEN_HEALTHY,
    YELLOW_BAD,
    RED_DEATH,
    BLUE_BONUS
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    float maxLifetime;
    Color color;
    float size;
};

struct ScorePopup {
    Vector2 position;
    int value;
    float lifetime;
    Color color;
    float scale;
};

struct Ball {
    Vector2 position;
    float radius;
    float speed;
    BallType type;
    Color color;
    float pulseTimer;
    float rotationAngle;
    bool isBeingEaten;
    float eatProgress;
    Vector2 targetPosition;
};

struct PaddleTrail {
    Vector2 position;
    float alpha;
};

class Game {
private:
    // Screen
    int screenWidth;
    int screenHeight;
    
    // Paddle (now vertical)
    float paddleX;
    float paddleTargetX;
    float paddleSpeed;
    float paddleLength;
    float paddleThickness;
    float paddleGlowIntensity;
    std::vector<PaddleTrail> paddleTrails;
    
    // Game state
    int score;
    int lastScore;
    int highScore;
    int health;
    int maxHealth;
    bool gameOver;
    bool gameStarted;
    float startAnimTimer;
    float gameOverAnimTimer;
    
    // Score popups
    std::vector<ScorePopup> scorePopups;
    
    // Balls
    std::vector<Ball> balls;
    float spawnTimer;
    float spawnInterval;
    float difficultyTimer;
    
    // Particles
    std::vector<Particle> particles;
    
    // Animation
    float timeElapsed;
    float screenShake;
    Vector2 shakeOffset;
    float screenShakeIntensity;
    
    // Camera
    Camera2D camera;
    
    // Combo system
    int comboCount;
    float comboTimer;
    
    // Color wave effect
    float colorWaveTimer;
    
public:
    Game() {
        screenWidth = 1920;
        screenHeight = 1080;
        
        InitWindow(screenWidth, screenHeight, "Vertical Paddle Eater - EXTREME!");
        ToggleFullscreen();
        SetTargetFPS(60);
        
        Reset();
        
        camera = { 0 };
        camera.target = { screenWidth / 2.0f, screenHeight / 2.0f };
        camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
    }
    
    void Reset() {
        paddleX = screenWidth / 2.0f;
        paddleTargetX = paddleX;
        paddleSpeed = 15.0f;
        paddleLength = screenHeight;
        paddleThickness = 35.0f;
        paddleGlowIntensity = 0.0f;
        
        score = 0;
        lastScore = 0;
        health = 3;
        maxHealth = 3;
        gameOver = false;
        gameStarted = false;
        startAnimTimer = 0.0f;
        gameOverAnimTimer = 0.0f;
        
        balls.clear();
        particles.clear();
        paddleTrails.clear();
        scorePopups.clear();
        
        spawnTimer = 0.0f;
        spawnInterval = 1.2f;
        difficultyTimer = 0.0f;
        timeElapsed = 0.0f;
        screenShake = 0.0f;
        screenShakeIntensity = 1.0f;
        
        comboCount = 0;
        comboTimer = 0.0f;
        colorWaveTimer = 0.0f;
        
        // Create intro particles
        for (int i = 0; i < 200; i++) {
            Particle p;
            p.position = {screenWidth / 2.0f, screenHeight / 2.0f};
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float speed = (float)GetRandomValue(100, 500);
            p.velocity = {cosf(angle) * speed, sinf(angle) * speed};
            p.lifetime = 0.0f;
            p.maxLifetime = (float)GetRandomValue(10, 30) / 10.0f;
            Color colors[] = {GREEN, SKYBLUE, PURPLE, GOLD, RED};
            p.color = colors[GetRandomValue(0, 4)];
            p.size = (float)GetRandomValue(5, 15);
            particles.push_back(p);
        }
    }
    
    void SpawnBall() {
        Ball newBall;
        newBall.position = {(float)GetRandomValue(80, screenWidth - 80), -50.0f};
        newBall.radius = (float)GetRandomValue(25, 45);
        newBall.speed = (float)GetRandomValue(250, 450);
        newBall.pulseTimer = 0.0f;
        newBall.rotationAngle = 0.0f;
        newBall.isBeingEaten = false;
        newBall.eatProgress = 0.0f;
        
        // Determine ball type with weighted probabilities
        int rand = GetRandomValue(0, 100);
        if (rand < 50) {
            newBall.type = GREEN_HEALTHY;
            newBall.color = Color{50, 255, 100, 255};
        } else if (rand < 75) {
            newBall.type = YELLOW_BAD;
            newBall.color = Color{255, 220, 50, 255};
        } else if (rand < 90) {
            newBall.type = BLUE_BONUS;
            newBall.color = Color{100, 150, 255, 255};
        } else {
            newBall.type = RED_DEATH;
            newBall.color = Color{255, 50, 50, 255};
        }
        
        balls.push_back(newBall);
    }
    
    void CreateParticles(Vector2 position, Color color, int count) {
        for (int i = 0; i < count; i++) {
            Particle p;
            p.position = position;
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float speed = (float)GetRandomValue(100, 400);
            p.velocity = {cosf(angle) * speed, sinf(angle) * speed};
            p.lifetime = 0.0f;
            p.maxLifetime = (float)GetRandomValue(5, 20) / 10.0f;
            p.color = color;
            p.size = (float)GetRandomValue(4, 12);
            particles.push_back(p);
        }
    }
    
    void AddScreenShake(float intensity) {
        screenShake = intensity * screenShakeIntensity;
    }
    
    void AddScorePopup(Vector2 position, int value, Color color) {
        ScorePopup popup;
        popup.position = position;
        popup.value = value;
        popup.lifetime = 0.0f;
        popup.color = color;
        popup.scale = 0.0f;
        scorePopups.push_back(popup);
    }
    
    void EatBall(Ball& ball) {
        switch(ball.type) {
            case GREEN_HEALTHY:
                score += 100;
                AddScorePopup(ball.position, 100, GREEN);
                CreateParticles(ball.position, GREEN, 50);
                paddleGlowIntensity = 2.0f;
                comboCount++;
                comboTimer = 2.0f;
                AddScreenShake(8.0f);
                break;
                
            case YELLOW_BAD:
                score = std::max(0, score - 50);
                AddScorePopup(ball.position, -50, YELLOW);
                CreateParticles(ball.position, YELLOW, 30);
                AddScreenShake(15.0f);
                comboCount = 0;
                break;
                
            case RED_DEATH:
                health--;
                AddScorePopup(ball.position, 0, RED);
                CreateParticles(ball.position, RED, 100);
                AddScreenShake(40.0f);
                comboCount = 0;
                
                // Extra screen shake particles
                for (int i = 0; i < 50; i++) {
                    CreateParticles({(float)GetRandomValue(0, screenWidth), 
                                   (float)GetRandomValue(0, screenHeight)}, 
                                   RED, 5);
                }
                
                if (health <= 0) {
                    gameOver = true;
                    gameOverAnimTimer = 0.0f;
                    if (score > highScore) {
                        highScore = score;
                    }
                    
                    // Game over explosion
                    for (int i = 0; i < 500; i++) {
                        Particle p;
                        p.position = {screenWidth / 2.0f, screenHeight / 2.0f};
                        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                        float speed = (float)GetRandomValue(200, 800);
                        p.velocity = {cosf(angle) * speed, sinf(angle) * speed};
                        p.lifetime = 0.0f;
                        p.maxLifetime = (float)GetRandomValue(10, 40) / 10.0f;
                        p.color = RED;
                        p.size = (float)GetRandomValue(5, 20);
                        particles.push_back(p);
                    }
                }
                break;
                
            case BLUE_BONUS:
                score += 200;
                AddScorePopup(ball.position, 200, SKYBLUE);
                health = std::min(health + 1, maxHealth);
                CreateParticles(ball.position, SKYBLUE, 60);
                paddleGlowIntensity = 3.0f;
                comboCount++;
                comboTimer = 2.0f;
                AddScreenShake(12.0f);
                
                // Bonus healing wave
                for (int i = 0; i < 30; i++) {
                    Particle p;
                    p.position = ball.position;
                    p.velocity = {0, (float)GetRandomValue(-200, -400)};
                    p.lifetime = 0.0f;
                    p.maxLifetime = 2.0f;
                    p.color = SKYBLUE;
                    p.size = 8.0f;
                    particles.push_back(p);
                }
                break;
        }
        
        lastScore = score;
    }
    
    void Update() {
        float deltaTime = GetFrameTime();
        timeElapsed += deltaTime;
        colorWaveTimer += deltaTime;
        
        // Start animation
        if (!gameStarted) {
            startAnimTimer += deltaTime;
            
            if (startAnimTimer > 3.0f || IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
            }
            
            // Pulsing effect
            camera.zoom = 1.0f + sinf(startAnimTimer * 3.0f) * 0.05f;
        }
        
        if (gameStarted && !gameOver) {
            camera.zoom = Lerp(camera.zoom, 1.0f, 0.1f);
            
            // Smooth paddle movement (now horizontal for vertical paddle)
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                paddleTargetX -= paddleSpeed;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                paddleTargetX += paddleSpeed;
            }
            
            paddleTargetX = Clamp(paddleTargetX, paddleThickness, screenWidth - paddleThickness);
            paddleX = Lerp(paddleX, paddleTargetX, 0.3f);
            
            // Add paddle trail
            if ((int)(timeElapsed * 60) % 2 == 0) {
                PaddleTrail trail;
                trail.position = {paddleX, screenHeight / 2.0f};
                trail.alpha = 255;
                paddleTrails.push_back(trail);
            }
            
            // Update trails
            for (size_t i = 0; i < paddleTrails.size(); i++) {
                paddleTrails[i].alpha -= 400 * deltaTime;
                if (paddleTrails[i].alpha <= 0) {
                    paddleTrails.erase(paddleTrails.begin() + i);
                    i--;
                }
            }
            
            // Decay glow
            paddleGlowIntensity = std::max(0.0f, paddleGlowIntensity - deltaTime * 3.0f);
            
            // Combo timer
            if (comboTimer > 0) {
                comboTimer -= deltaTime;
                if (comboTimer <= 0) {
                    comboCount = 0;
                }
            }
            
            // Difficulty increase
            difficultyTimer += deltaTime;
            if (difficultyTimer >= 8.0f) {
                spawnInterval = std::max(0.3f, spawnInterval - 0.08f);
                difficultyTimer = 0.0f;
            }
            
            // Spawn balls
            spawnTimer += deltaTime;
            if (spawnTimer >= spawnInterval) {
                SpawnBall();
                spawnTimer = 0.0f;
            }
            
            // Update balls
            for (size_t i = 0; i < balls.size(); i++) {
                Ball& ball = balls[i];
                ball.pulseTimer += deltaTime * 3.0f;
                ball.rotationAngle += deltaTime * 100.0f;
                
                if (!ball.isBeingEaten) {
                    ball.position.y += ball.speed * deltaTime;
                    
                    // Check if ball is at paddle Y position
                    if (ball.position.y >= screenHeight / 2.0f - 50 && 
                        ball.position.y <= screenHeight / 2.0f + 50) {
                        
                        // Check collision with paddle
                        float distance = fabsf(ball.position.x - paddleX);
                        
                        if (distance < ball.radius + paddleThickness / 2) {
                            // Start eating animation
                            ball.isBeingEaten = true;
                            ball.targetPosition = {paddleX, screenHeight / 2.0f};
                        }
                    }
                    
                    // Remove if off-screen
                    if (ball.position.y - ball.radius > screenHeight) {
                        balls.erase(balls.begin() + i);
                        i--;
                    }
                } else {
                    // Eating animation
                    ball.eatProgress += deltaTime * 5.0f;
                    
                    // Move towards paddle center
                    ball.position.x = Lerp(ball.position.x, ball.targetPosition.x, 0.3f);
                    ball.position.y = Lerp(ball.position.y, ball.targetPosition.y, 0.3f);
                    ball.radius = Lerp(ball.radius, 0.0f, 0.2f);
                    
                    if (ball.eatProgress >= 1.0f || ball.radius < 2.0f) {
                        EatBall(ball);
                        balls.erase(balls.begin() + i);
                        i--;
                    }
                }
            }
            
            // Update score popups
            for (size_t i = 0; i < scorePopups.size(); i++) {
                scorePopups[i].lifetime += deltaTime;
                scorePopups[i].position.y -= 100 * deltaTime;
                
                // Scale animation
                if (scorePopups[i].lifetime < 0.2f) {
                    scorePopups[i].scale = scorePopups[i].lifetime / 0.2f;
                } else {
                    scorePopups[i].scale = 1.0f;
                }
                
                if (scorePopups[i].lifetime > 2.0f) {
                    scorePopups.erase(scorePopups.begin() + i);
                    i--;
                }
            }
            
            // Update particles
            for (size_t i = 0; i < particles.size(); i++) {
                particles[i].lifetime += deltaTime;
                particles[i].position.x += particles[i].velocity.x * deltaTime;
                particles[i].position.y += particles[i].velocity.y * deltaTime;
                particles[i].velocity.y += 400.0f * deltaTime; // Gravity
                
                if (particles[i].lifetime >= particles[i].maxLifetime) {
                    particles.erase(particles.begin() + i);
                    i--;
                }
            }
            
            // EXTREME Screen shake
            if (screenShake > 0) {
                screenShake -= deltaTime * 40.0f;
                float intensity = screenShake * 2.5f; // Amplified shake
                shakeOffset.x = (float)GetRandomValue(-20, 20) * intensity * 0.15f;
                shakeOffset.y = (float)GetRandomValue(-20, 20) * intensity * 0.15f;
                
                // Add shake rotation
                camera.rotation = sinf(timeElapsed * 30.0f) * screenShake * 0.5f;
            } else {
                shakeOffset = {0, 0};
                camera.rotation = Lerp(camera.rotation, 0.0f, 0.1f);
            }
            
        } else if (gameOver) {
            gameOverAnimTimer += deltaTime;
            
            // Zoom out and rotate
            camera.zoom = Lerp(camera.zoom, 0.8f, 0.05f);
            camera.rotation += deltaTime * 20.0f;
            
            // Continue particle updates
            for (size_t i = 0; i < particles.size(); i++) {
                particles[i].lifetime += deltaTime;
                particles[i].position.x += particles[i].velocity.x * deltaTime;
                particles[i].position.y += particles[i].velocity.y * deltaTime;
                particles[i].velocity.y += 400.0f * deltaTime;
                
                if (particles[i].lifetime >= particles[i].maxLifetime) {
                    particles.erase(particles.begin() + i);
                    i--;
                }
            }
            
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                Reset();
            }
        }
    }
    
    void Draw() {
        BeginDrawing();
        
        // Animated background color
        int bgR = 10 + (int)(sinf(colorWaveTimer * 0.5f) * 5);
        int bgG = 10 + (int)(cosf(colorWaveTimer * 0.3f) * 5);
        int bgB = 20 + (int)(sinf(colorWaveTimer * 0.7f) * 10);
        ClearBackground(Color{(unsigned char)bgR, (unsigned char)bgG, (unsigned char)bgB, 255});
        
        camera.offset.x = screenWidth / 2.0f + shakeOffset.x;
        camera.offset.y = screenHeight / 2.0f + shakeOffset.y;
        
        BeginMode2D(camera);
        
        // Draw background grid with animation
        for (int i = 0; i < screenWidth; i += 50) {
            int alpha = 30 + (int)(sinf(colorWaveTimer + i * 0.01f) * 20);
            DrawLine(i, 0, i, screenHeight, Color{30, 30, 45, (unsigned char)alpha});
        }
        for (int i = 0; i < screenHeight; i += 50) {
            int alpha = 30 + (int)(cosf(colorWaveTimer + i * 0.01f) * 20);
            DrawLine(0, i, screenWidth, i, Color{30, 30, 45, (unsigned char)alpha});
        }
        
        // Draw particles
        for (const auto& p : particles) {
            float alpha = 1.0f - (p.lifetime / p.maxLifetime);
            Color col = p.color;
            col.a = (unsigned char)(alpha * 255);
            DrawCircleV(p.position, p.size * alpha, col);
            
            // Add glow
            col.a = (unsigned char)(alpha * 80);
            DrawCircleV(p.position, p.size * alpha * 2, col);
        }
        
        // Draw paddle trails (now vertical)
        for (const auto& trail : paddleTrails) {
            Color trailColor = SKYBLUE;
            trailColor.a = (unsigned char)trail.alpha;
            DrawLineEx({trail.position.x, 0}, {trail.position.x, (float)screenHeight}, 
                      paddleThickness * 0.7f, trailColor);
        }
        
        // Draw vertical paddle with extreme glow
        float glowSize = paddleThickness + 30.0f + paddleGlowIntensity * 50.0f;
        
        // Multiple glow layers
        for (int i = 5; i > 0; i--) {
            float layerSize = glowSize * (i / 5.0f);
            int layerAlpha = 20 * i;
            DrawLineEx({paddleX, 0}, {paddleX, (float)screenHeight}, 
                      layerSize, Color{100, 200, 255, (unsigned char)layerAlpha});
        }
        
        DrawLineEx({paddleX, 0}, {paddleX, (float)screenHeight}, 
                  paddleThickness + 10, Color{100, 200, 255, 150});
        
        DrawLineEx({paddleX, 0}, {paddleX, (float)screenHeight}, 
                  paddleThickness, Color{150, 220, 255, 255});
        
        // Animated energy waves on paddle
        for (int i = 0; i < 3; i++) {
            float waveY = fmodf(timeElapsed * 300.0f + i * 200.0f, (float)screenHeight);
            DrawCircle(paddleX, waveY, paddleThickness * 0.8f, Color{255, 255, 255, 100});
        }
        
        // Draw balls
        for (const auto& ball : balls) {
            float pulseSize = sinf(ball.pulseTimer) * 6.0f;
            
            // Multiple outer glows
            for (int i = 3; i > 0; i--) {
                DrawCircleV(ball.position, ball.radius + pulseSize + (i * 15), 
                           Color{ball.color.r, ball.color.g, ball.color.b, (unsigned char)(20 * i)});
            }
            
            // Main ball
            DrawCircleV(ball.position, ball.radius + pulseSize, ball.color);
            
            // Inner highlight
            Color highlightColor = WHITE;
            highlightColor.a = 180;
            DrawCircleV({ball.position.x - ball.radius * 0.3f, 
                        ball.position.y - ball.radius * 0.3f}, 
                       ball.radius * 0.4f, highlightColor);
            
            // Rotating ring
            DrawRing(ball.position, ball.radius + pulseSize + 5, ball.radius + pulseSize + 8,
                    ball.rotationAngle, ball.rotationAngle + 180, 20, WHITE);
            
            // Draw icon based on type
            const char* icon = "";
            switch(ball.type) {
                case GREEN_HEALTHY: icon = "+"; break;
                case YELLOW_BAD: icon = "-"; break;
                case RED_DEATH: icon = "X"; break;
                case BLUE_BONUS: icon = "*"; break;
            }
            
            int fontSize = (int)(ball.radius * 1.5f);
            Vector2 textSize = MeasureTextEx(GetFontDefault(), icon, fontSize, 2);
            DrawText(icon, 
                    ball.position.x - textSize.x / 2, 
                    ball.position.y - textSize.y / 2, 
                    fontSize, BLACK);
        }
        
        // Draw score popups
        for (const auto& popup : scorePopups) {
            float alpha = 1.0f - (popup.lifetime / 2.0f);
            Color popupColor = popup.color;
            popupColor.a = (unsigned char)(alpha * 255);
            
            const char* text = TextFormat("%+d", popup.value);
            int fontSize = (int)(60 * popup.scale);
            Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 2);
            
            // Shadow
            DrawText(text, popup.position.x - textSize.x / 2 + 3, 
                    popup.position.y - textSize.y / 2 + 3, fontSize, BLACK);
            
            // Main text
            DrawText(text, popup.position.x - textSize.x / 2, 
                    popup.position.y - textSize.y / 2, fontSize, popupColor);
        }
        
        EndMode2D();
        
        // Start screen
        if (!gameStarted) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 150});
            
            float pulse = 1.0f + sinf(startAnimTimer * 5.0f) * 0.2f;
            
            const char* titleText = "VERTICAL PADDLE EATER";
            int textWidth = MeasureText(titleText, 100);
            
            // Title with glow
            for (int i = 0; i < 5; i++) {
                DrawText(titleText, screenWidth / 2 - textWidth / 2, 
                        screenHeight / 2 - 200, 100, Color{100, 200, 255, (unsigned char)(30 * i)});
            }
            DrawText(titleText, screenWidth / 2 - textWidth / 2, 
                    screenHeight / 2 - 200, 100, SKYBLUE);
            
            const char* startText = "Press SPACE to Start";
            textWidth = MeasureText(startText, (int)(50 * pulse));
            DrawText(startText, screenWidth / 2 - textWidth / 2, 
                    screenHeight / 2, (int)(50 * pulse), WHITE);
            
            // Controls
            DrawText("LEFT/RIGHT or A/D - Move Paddle", screenWidth / 2 - 300, 
                    screenHeight / 2 + 100, 30, LIGHTGRAY);
        }
        
        // Draw UI
        if (gameStarted && !gameOver) {
            // Score with scale animation
            float scoreScale = 1.0f;
            if (score != lastScore) {
                scoreScale = 1.5f + sinf(timeElapsed * 20.0f) * 0.3f;
            }
            
            const char* scoreText = TextFormat("SCORE: %d", score);
            DrawText(scoreText, 30, 30, (int)(60 * scoreScale), WHITE);
            DrawText(TextFormat("HIGH: %d", highScore), 30, 100, 30, GRAY);
            
            // Draw health with animation
            for (int i = 0; i < maxHealth; i++) {
                float heartPulse = i < health ? 1.0f + sinf(timeElapsed * 5.0f + i) * 0.2f : 1.0f;
                Color heartColor = i < health ? RED : Color{60, 60, 70, 255};
                DrawCircle(screenWidth - 60 - (i * 50), 50, 18 * heartPulse, heartColor);
                
                if (i < health) {
                    DrawCircle(screenWidth - 60 - (i * 50), 50, 22 * heartPulse, 
                              Color{255, 100, 100, 80});
                }
            }
            
            // Draw combo
            if (comboCount > 1) {
                float comboScale = 1.0f + sinf(timeElapsed * 10.0f) * 0.3f;
                const char* comboText = TextFormat("COMBO x%d!", comboCount);
                int textWidth = MeasureText(comboText, (int)(80 * comboScale));
                float comboAlpha = comboTimer / 2.0f;
                Color comboColor = GOLD;
                comboColor.a = (unsigned char)(comboAlpha * 255);
                
                // Glow effect
                for (int i = 0; i < 3; i++) {
                    DrawText(comboText, screenWidth / 2 - textWidth / 2, 150, 
                            (int)(80 * comboScale), Color{255, 215, 0, (unsigned char)(50 * i)});
                }
                
                DrawText(comboText, screenWidth / 2 - textWidth / 2, 150, 
                        (int)(80 * comboScale), comboColor);
            }
            
            // Draw legend
            DrawText("GREEN(+) = +100pts", 30, screenHeight - 130, 25, GREEN);
            DrawText("BLUE(*) = +200pts +HP", 30, screenHeight - 100, 25, SKYBLUE);
            DrawText("YELLOW(-) = -50pts", 30, screenHeight - 70, 25, YELLOW);
            DrawText("RED(X) = DEATH", 30, screenHeight - 40, 25, RED);
        }
        
        // Game over screen with animation
        if (gameOver) {
            int overlayAlpha = (int)std::min(200.0f, gameOverAnimTimer * 100.0f);
            DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, (unsigned char)overlayAlpha});
            
            if (gameOverAnimTimer > 1.0f) {
                float textScale = std::min(1.0f, (gameOverAnimTimer - 1.0f) * 2.0f);
                float pulse = 1.0f + sinf(gameOverAnimTimer * 5.0f) * 0.1f;
                
                const char* gameOverText = "GAME OVER!";
                int textWidth = MeasureText(gameOverText, (int)(120 * textScale * pulse));
                
                // Extreme glow
                for (int i = 0; i < 10; i++) {
                    DrawText(gameOverText, screenWidth / 2 - textWidth / 2, 
                            screenHeight / 2 - 150, (int)(120 * textScale * pulse), 
                            Color{255, 0, 0, (unsigned char)(20 * i)});
                }
                
                DrawText(gameOverText, screenWidth / 2 - textWidth / 2, 
                        screenHeight / 2 - 150, (int)(120 * textScale * pulse), RED);
                
                const char* scoreText = TextFormat("Final Score: %d", score);
                textWidth = MeasureText(scoreText, (int)(70 * textScale));
                DrawText(scoreText, screenWidth / 2 - textWidth / 2, 
                        screenHeight / 2 - 20, (int)(70 * textScale), WHITE);
                
                if (score == highScore && score > 0) {
                    float goldPulse = 1.0f + sinf(gameOverAnimTimer * 8.0f) * 0.3f;
                    const char* newRecord = "NEW HIGH SCORE!";
                    textWidth = MeasureText(newRecord, (int)(50 * textScale * goldPulse));
                    
                    for (int i = 0; i < 5; i++) {
                        DrawText(newRecord, screenWidth / 2 - textWidth / 2, 
                                screenHeight / 2 + 60, (int)(50 * textScale * goldPulse), 
                                Color{255, 215, 0, (unsigned char)(40 * i)});
                    }
                    
                    DrawText(newRecord, screenWidth / 2 - textWidth / 2, 
                            screenHeight / 2 + 60, (int)(50 * textScale * goldPulse), GOLD);
                }
                
                const char* restartText = "Press SPACE or ENTER to restart";
                textWidth = MeasureText(restartText, (int)(40 * textScale));
                DrawText(restartText, screenWidth / 2 - textWidth / 2, 
                        screenHeight / 2 + 140, (int)(40 * textScale), LIGHTGRAY);
            }
        }
        
        EndDrawing();
    }
    
    void Run() {
        while (!WindowShouldClose()) {
            Update();
            Draw();
        }
    }
    
    ~Game() {
        CloseWindow();
    }
};

int main() {
    Game game;
    game.Run();
    return 0;
}