#include "raylib.h"
#include "raymath.h" // Required for Lerp
#include <vector>

enum BallType { HEALTHY, SICK, DEADLY };

struct Ball {
    Vector2 position;
    float radius;
    float speed;
    BallType type;
    bool active;
};

// Simple Particle for "Awesome" visuals
struct Particle {
    Vector2 pos;
    Vector2 vel;
    Color color;
    float alpha;
};

int main() {
    // Initialization
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Neon Collector - Raylib 5.0");
    ToggleFullscreen();

    // Game Variables
    float paddleY = GetScreenHeight() / 2.0f;
    float targetY = paddleY; 
    float paddleWidth = 15.0f;
    float paddleHeight = 100.0f;
    
    std::vector<Ball> balls;
    std::vector<Particle> particles;
    float spawnTimer = 0;
    int score = 0;
    bool gameOver = false;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();

        if (!gameOver) {
            // 1. Smooth Movement (Lerp)
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) targetY -= 600 * dt;
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) targetY += 600 * dt;
            targetY = Clamp(targetY, 0, screenH - paddleHeight);
            // The magic: paddle follows target with 15% closure per frame
            paddleY = Lerp(paddleY, targetY, 0.15f);

            // 2. Spawning Logic
            spawnTimer += dt;
            if (spawnTimer > 0.8f) {
                int roll = GetRandomValue(0, 10);
                BallType t = (roll < 6) ? HEALTHY : (roll < 9 ? SICK : DEADLY);
                balls.push_back({{screenW + 50, (float)GetRandomValue(50, screenH - 50)}, 
                                 (float)GetRandomValue(12, 22), 
                                 (float)GetRandomValue(350, 550), t, true});
                spawnTimer = 0;
            }

            // 3. Update Balls & Particles
            for (int i = 0; i < balls.size(); i++) {
                balls[i].position.x -= balls[i].speed * dt;

                // Collision Check (AABB vs Circle Lite)
                Rectangle paddleRect = { 100, paddleY, paddleWidth, paddleHeight };
                if (CheckCollisionCircleRec(balls[i].position, balls[i].radius, paddleRect)) {
                    // Collision Effects
                    Color pColor = WHITE;
                    if (balls[i].type == HEALTHY) { score += 50; pColor = LIME; }
                    else if (balls[i].type == SICK) { score -= 100; pColor = GOLD; }
                    else if (balls[i].type == DEADLY) { gameOver = true; pColor = RED; }

                    // Explode into particles
                    for(int j=0; j<10; j++) 
                        particles.push_back({balls[i].position, {(float)GetRandomValue(-5, 2), (float)GetRandomValue(-3, 3)}, pColor, 1.0f});
                    
                    balls.erase(balls.begin() + i);
                    i--;
                }
            }

            // Update Particles
            for (int i = 0; i < particles.size(); i++) {
                particles[i].pos.x += particles[i].vel.x;
                particles[i].pos.y += particles[i].vel.y;
                particles[i].alpha -= 0.02f;
                if (particles[i].alpha <= 0) { particles.erase(particles.begin() + i); i--; }
            }
        }

        // 4. Draw
        BeginDrawing();
            ClearBackground({10, 10, 15, 255}); // Dark Navy

            // Draw Particles
            for (auto& p : particles) DrawCircleV(p.pos, 3, Fade(p.color, p.alpha));

            // Draw Paddle with Glow
            DrawRectangleRounded({100, paddleY, paddleWidth, paddleHeight}, 0.8f, 10, RED);
            DrawRectangleRoundedLines({100, paddleY, paddleWidth, paddleHeight}, 0.8f, 10, WHITE);

            // Draw Balls
            for (const auto& b : balls) {
                Color c = (b.type == HEALTHY) ? LIME : (b.type == SICK ? GOLD : RED);
                DrawCircleGradient(b.position.x, b.position.y, b.radius, c, Fade(BLACK, 0.3f));
                DrawCircleLines(b.position.x, b.position.y, b.radius + (sin(GetTime()*10)*2), Fade(c, 0.5f));
            }

            DrawText(TextFormat("SCORE: %04i", score), 40, 40, 30, RAYWHITE);
            if (gameOver) {
                DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.8f));
                DrawText("CRITICAL FAILURE", screenW/2 - 180, screenH/2 - 40, 40, RED);
                DrawText("Press 'R' to Reboot", screenW/2 - 100, screenH/2 + 20, 20, GRAY);
                if (IsKeyPressed(KEY_R)) { balls.clear(); score = 0; gameOver = false; }
            }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}