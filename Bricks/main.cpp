#include <random>
#include "raylib.h"
#include "game.h"
#include "constants.h"

#ifdef NDEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


void DrawBrick(const Bricks::Brick& b) {

    int width = b.GetLength();
    int height = b.GetLength();
    Color c = b.GetColor();
    float x = b.GetPos().x;
    float y = b.GetPos().y;

    float margin = width * 0.2f;
    float fade = 0.2f;
    Color inner_c = { (unsigned char)(c.r * fade), (unsigned char)(c.g * fade), (unsigned char)(c.b * fade), (unsigned char)(c.a * fade) };
    Color outer_c = { c.r, c.g, c.b, c.a };
    //outer rec
    Rectangle outer_rec = { x , y , static_cast<float>(width) , static_cast<float>(height) };
    DrawRectangleRounded(outer_rec, 0.1f, 16, outer_c);
    //inner rec
    Rectangle inner_rec = { x + margin, y+ margin, width - (2* margin), height - (2* margin)};
    DrawRectangleRounded(inner_rec, 0.1f, 16, inner_c);

    // Bright neon border
    for (int i = 8; i > 0; i--) {
        Color glow = c;
        glow.a = 20;  // very transparent
        DrawRectangleRoundedLinesEx(outer_rec, 0.1f, 16, (float)i * 2, glow);
    }
    // main bright outline
    DrawRectangleRoundedLinesEx(outer_rec, 0.1f, 16, 4, WHITE);
    int textSize = MeasureText("100", (width * width)/100);
    DrawText(TextFormat("%02i", b.GetHealth()), (x+width/2) - (textSize/2), (y+height/2) - (textSize/2), (width * width) / 100, RED);
}

void DrawCanonBall(const Bricks::CanonBall& ball) {
    DrawCircle(ball.GetPos().x, ball.GetPos().y, ball.GetRadius(), ball.GetColor());
}

Vector2 NormalizeVector(const Vector2& v) {
    float len = sqrtf(v.x * v.x + v.y * v.y);
    if (len == 0) {
        return { 0.0f,0.0f };

    } 
    return { v.x / len, v.y / len };
}

bool floatEqual(float a, float b) {
    return std::fabs(a - b) < EPSILON;
}

bool CheckCirclePointCollision(Vector2 circlePos, float r, Vector2 point) {
    float dx = circlePos.x - point.x;
    float dy = circlePos.y - point.y;
    return (dx * dx + dy * dy) <= r * r;
}

void ReflectFromPoint(Bricks::CanonBall& ball, Vector2 point) {
    Vector2 pos = ball.GetPos();
    Vector2 normal = { pos.x - point.x, pos.y - point.y };

    float len = sqrtf(normal.x * normal.x + normal.y * normal.y);
    if (len == 0) return; // ball center exactly at corner
    normal.x /= len;
    normal.y /= len;

    Vector2 dir = ball.GetDir();
    float dot = dir.x * normal.x + dir.y * normal.y;

    Vector2 newDir = {
        dir.x - 2 * dot * normal.x,
        dir.y - 2 * dot * normal.y
    };

    ball.SetDir(newDir);
    ball.SetPos({ point.x + normal.x * ball.GetRadius(),
                  point.y + normal.y * ball.GetRadius() });
}


bool CheckCircleRecCollision(Bricks::CanonBall& ball, Bricks::Brick& brick) {
    Vector2 pos = ball.GetPos();
    float r = ball.GetRadius();

    float left = brick.GetPos().x;
    float right = brick.GetPos().x + brick.GetLength();
    float top = brick.GetPos().y;
    float bottom = brick.GetPos().y + brick.GetLength();

    // --- Left side ---
    if (pos.x + r >= left &&
        pos.x < left &&  // ensure ball is really on the left side
        pos.y + r >= top &&
        pos.y - r <= bottom) {
        ball.SetPos({ left - r, pos.y });
        ball.SetDir({ -ball.GetDir().x, ball.GetDir().y });
        return true;
    }

    // --- Right side ---
    if (pos.x - r <= right &&
        pos.x > right &&  // ensure ball is really on the right side
        pos.y + r >= top &&
        pos.y - r <= bottom) {
        ball.SetPos({ right + r, pos.y });
        ball.SetDir({ -ball.GetDir().x, ball.GetDir().y });
        return true;
    }

    // --- Top side ---
    if (pos.y + r >= top &&
        pos.y < top &&  // ensure ball is really above
        pos.x + r >= left &&
        pos.x - r <= right) {
        ball.SetPos({ pos.x, top - r });
        ball.SetDir({ ball.GetDir().x, -ball.GetDir().y });
        return true;
    }

    // --- Bottom side ---
    if (pos.y - r <= bottom &&
        pos.y > bottom &&  // ensure ball is really below
        pos.x + r >= left &&
        pos.x - r <= right) {
        ball.SetPos({ pos.x, bottom + r });
        ball.SetDir({ ball.GetDir().x, -ball.GetDir().y });
        return true;
    }

    // --- Corners ---
    // Top-left corner
    if (CheckCirclePointCollision(pos, r, { left, top })) {
        ReflectFromPoint(ball, { left, top });
        return true;
    }
    // Top-right corner
    if (CheckCirclePointCollision(pos, r, { right, top })) {
        ReflectFromPoint(ball, { right, top });
        return true;
    }
    // Bottom-left corner
    if (CheckCirclePointCollision(pos, r, { left, bottom })) {
        ReflectFromPoint(ball, { left, bottom });
        return true;
    }
    // Bottom-right corner
    if (CheckCirclePointCollision(pos, r, { right, bottom })) {
        ReflectFromPoint(ball, { right, bottom });
        return true;
    }

    return false;
}


void HandleCircleBoundsCollision(Bricks::CanonBall& ball, Vector2 startPos) {
    // Left boundary
    if (ball.GetPos().x - ball.GetRadius() <= PADDING) {
        ball.SetPos({ ball.GetRadius() + PADDING, ball.GetPos().y });
        ball.SetDir({ -ball.GetDir().x, ball.GetDir().y });
    }
    // Right boundary
    if (ball.GetPos().x + ball.GetRadius() >= SCREEN_WIDTH - PADDING) {
        ball.SetPos({ SCREEN_WIDTH - PADDING - ball.GetRadius(), ball.GetPos().y });
        ball.SetDir({ -ball.GetDir().x, ball.GetDir().y });
    }

    // Top boundary
    if (ball.GetPos().y - ball.GetRadius() <= PADDING) {
        ball.SetPos({ ball.GetPos().x, ball.GetRadius() + PADDING });
        ball.SetDir({ ball.GetDir().x, -ball.GetDir().y });
    }

    // Bottom boundary
    if (ball.GetPos().y + ball.GetRadius() >= SCREEN_HEIGHT - PADDING) {
        ball.SetPos({ ball.GetPos().x, SCREEN_HEIGHT - PADDING - ball.GetRadius() });

        // Instead of bouncing, reset ball state
        ball.SetState(Bricks::BallState::Return);
        ball.SetPrevState(Bricks::BallState::Flight);

        Vector2 dir = { startPos.x - ball.GetPos().x, startPos.y - ball.GetPos().y };
        dir = NormalizeVector(dir);
        ball.SetDir(dir);
    }
}




int main() {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bricks");
    SetTargetFPS(200);

    Bricks::Game game;

    std::vector<Bricks::CanonBall> balls = {};

    Bricks::Brick bricks[64];

    std::random_device rd;  // seed
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<> dist(0, 4); 
    Color colors[5] = { RED, BLUE, GREEN, YELLOW, VIOLET };

    std::uniform_real_distribution<> dist_uni(0.0, 1.0);

    Vector2 startPos = { PADDING + (GAME_WIDTH / 2), PADDING + GAME_HEIGHT };

    for (int i = 0; i < 100; i++) {
        Bricks::CanonBall ball = Bricks::CanonBall
        (
            startPos
          , {1.0f, -1.0f}
          , 5.0f
          , WHITE
          , i * 0.05
        );
        ball.SetVelocity(5.0f);
        balls.emplace_back(ball);
    }

    
    int square_cols = 8;
    int square_rows = 8;
    int num_squares = square_cols * square_rows;
   
    Vector2 GridPos = { 250, 100 };

    int brick_margin = 10;

    Bricks::GameState s = game.GetGameState();

    for (int i = 0; i < square_rows; ++i) {
        for (int j = 0; j < square_cols; ++j) {
            if (dist_uni(gen) < 0.8) {
                Vector2 brick_pos = { GridPos.x + (j * (BRICK_LENGTH + brick_margin))  , GridPos.y + (i * (BRICK_LENGTH + brick_margin)) };
                Bricks::Brick brick = Bricks::Brick(BRICK_LENGTH, 100, colors[dist(gen)], brick_pos);
                int index = i * square_rows + j;
                bricks[index] = brick;
                s.bricksALive++;
            }
            
        }
    }
    game.SetGameState(s);


    Rectangle game_outline = { PADDING, PADDING, SCREEN_WIDTH - (2 * PADDING), SCREEN_HEIGHT - (2 * PADDING) };

    double shootTime = -1;

    bool game_over = false;

    const char* game_over_text = "Game Over. Press any key to start again";


    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && game.GetGameState().ballsReturned) {

            shootTime = GetTime();
            Vector2 mousePos = GetMousePosition();

            if (game.GetGameState().bricksALive > 0) {
                
                for (int i = 0; i < balls.size(); ++i) {
                    if (balls[i].GetState() == Bricks::BallState::Resting) {
                        Vector2 dir = { mousePos.x - balls[i].GetPos().x , mousePos.y - balls[i].GetPos().y };
                        dir = NormalizeVector(dir);
                        balls[i].SetDir(dir);
                        balls[i].SetLag(i * 0.05);
                        balls[i].SetState(Bricks::BallState::Launched);
                        balls[i].SetPrevState(Bricks::BallState::Resting);
                    }
                }
                
            }
            else {
                Bricks::GameState s = game.GetGameState();

                s.bricksALive = 0;

                for (int i = 0; i < square_rows; ++i) {
                    for (int j = 0; j < square_cols; ++j) {
                        if (dist_uni(gen) < 0.8) {
                            Vector2 brick_pos = { GridPos.x + (j * (BRICK_LENGTH + brick_margin))  , GridPos.y + (i * (BRICK_LENGTH + brick_margin)) };
                            Bricks::Brick brick = Bricks::Brick(BRICK_LENGTH, 100, colors[dist(gen)], brick_pos);
                            int index = i * square_rows + j;
                            bricks[index] = brick;
                            s.bricksALive++;
                        }
                    }
                }
                game.SetGameState(s);
            }
  
        }

        double now = GetTime();

        for (int i = 0; i < 100; ++i) {

            if (balls[i].GetState() == Bricks::BallState::Launched ||
                balls[i].GetState() == Bricks::BallState::Flight ||
                balls[i].GetState() == Bricks::BallState::Return) {
                
                if (now >= shootTime + balls[i].GetLag()) {
                    balls[i].Move(balls[i].GetVelocity(), balls[i].GetDir());
                }


                if (!CheckCollisionPointCircle(startPos, balls[i].GetPos(), balls[i].GetRadius())) {
                    balls[i].SetState(Bricks::BallState::Flight);
                    balls[i].SetPrevState(Bricks::BallState::Launched);
                    game.SetBallsState(false);
                }

                if (CheckCollisionPointCircle(startPos, balls[i].GetPos(), balls[i].GetRadius()) && (balls[i].GetState() == Bricks::BallState::Return || balls[i].GetState() == Bricks::BallState::Flight)) {
                    balls[i].SetState(Bricks::BallState::Resting);
                    balls[i].SetPrevState(Bricks::BallState::Return);
                    balls[i].SetPos(startPos);
                }

                HandleCircleBoundsCollision(balls[i], startPos);

                
                for (int j = 0; j < 64; ++j) {

                    if (bricks[j].GetState() == Bricks::BrickState::Alive) {

                        if (CheckCircleRecCollision(balls[i], bricks[j])) {

                            bricks[j].SetHealth(bricks[j].GetHealth() - 1);
                            if (bricks[j].GetHealth() <= 0) {
                                bricks[j].SetState(Bricks::BrickState::Dead);
                                Bricks::GameState s = game.GetGameState();
                                s.bricksALive--;
                                game.SetGameState(s);
                            }

                        }

                    }
                }



            }
        }


        
        for (auto& ball : balls) {
            Bricks::BallState ball_state = ball.GetState();
            Bricks::BallState ball_prev_state = ball.GetPrevState();

            if (ball_state == Bricks::BallState::Resting && ball_prev_state != Bricks::BallState::Resting) {
                // ball has *just* transitioned to Resting
                Bricks::GameState s = game.GetGameState();
                s.numBallsReturned++;
                game.SetGameState(s);

                // mark prev state so it won't be counted again
                ball.SetPrevState(Bricks::BallState::Resting);
            }

        }

        if (game.GetGameState().numBallsReturned == balls.size()) {
            game.SetBallsState(true);
            Bricks::GameState s = game.GetGameState();
            s.numBallsReturned = 0;
            game.SetGameState(s);
        }

        
        BeginDrawing();

        ClearBackground(BLACK);

        DrawRectangleRoundedLinesEx(game_outline, 0.05f, 16, 4.0f, DARKGRAY);

        Vector2 inputPos = { (float)GetMouseX(), (float)GetMouseY() };

        if (GetTouchPointCount() > 0) {
            inputPos = GetTouchPosition(0);   // first finger
        }

        DrawLineV(startPos, inputPos, { 163, 178, 178, 114 });

        for (const auto& brick : bricks) {
            if (brick.GetState() == Bricks::BrickState::Alive) {
                DrawBrick(brick);
            }
        }

        for (int i = 0; i < balls.size(); i++) {

            DrawCanonBall(balls[i]);
        }

        if (game.GetGameState().bricksALive == 0) {            
            int textSize = MeasureText(game_over_text, 20);
            DrawText(game_over_text, (GAME_WIDTH / 2) - (textSize / 2), (GAME_HEIGHT / 2) , 20, RED);
        }

        EndDrawing();
    }

    CloseWindow();
	return 0;
}
