#include <iostream>
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



int main() {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bricks");
    SetTargetFPS(60);

    Bricks::Game game;

    std::vector<Bricks::CanonBall> balls = {};

    std::vector<Bricks::Brick> bricks = {};

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
          , i * 5
        );
        ball.SetVelocity(10.0f);
        balls.emplace_back(ball);
    }

    
    int square_cols = 8;
    int square_rows = 8;
    int num_squares = square_cols * square_rows;
   
    Vector2 GridPos = { 250, 100 };

    int brick_margin = 10;

    for (int i = 0; i < square_rows; ++i) {
        for (int j = 0; j < square_cols; ++j) {
            if (dist_uni(gen) < 0.8) {
                Vector2 brick_pos = { GridPos.x + (j * (BRICK_LENGTH + brick_margin))  , GridPos.y + (i * (BRICK_LENGTH + brick_margin)) };
                Bricks::Brick brick = Bricks::Brick(BRICK_LENGTH, 100, colors[dist(gen)], brick_pos);
                bricks.emplace_back(brick);
            }
            
        }
    }

    Bricks::GameState s = game.GetGameState();
    s.bricksALive = bricks.size();
    game.SetGameState(s);


    Rectangle game_outline = { PADDING, PADDING, SCREEN_WIDTH - (2 * PADDING), SCREEN_HEIGHT - (2 * PADDING) };

    double shootTime = -1;

    while (!WindowShouldClose()) {

        Bricks::GameState s = game.GetGameState();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && s.ballsReturned && s.bricksALive > 0) {

            Vector2 mousePos = GetMousePosition();

            shootTime = GetTime();

            for (int i = 0; i < balls.size(); ++i) {

                Bricks::BallState ball_state = balls[i].GetState();

                if (ball_state == Bricks::BallState::Resting) {
                    Vector2 dir = { mousePos.x - balls[i].GetPos().x , mousePos.y - balls[i].GetPos().y };
                    dir = NormalizeVector(dir);
                    balls[i].SetDir(dir);
                    balls[i].SetLag(i * 0.05);
                    balls[i].SetState(Bricks::BallState::Launched);
                    balls[i].SetPrevState(Bricks::BallState::Resting);
                }  
            }
        }

        double now = GetTime();

        for (auto& ball : balls) {

            Bricks::BallState ball_state = ball.GetState();

            if (ball_state == Bricks::BallState::Launched || ball_state == Bricks::BallState::Flight || ball_state == Bricks::BallState::Return) {

                if (now >= shootTime + ball.GetLag()) {
                    ball.Move(ball.GetVelocity(), ball.GetDir());

                }

                if (!CheckCollisionPointCircle(startPos, ball.GetPos(), ball.GetRadius())) {
                    ball.SetState(Bricks::BallState::Flight);
                    ball.SetPrevState(Bricks::BallState::Launched);
                    game.SetBallsState(false);
                }

                if (CheckCollisionCircleLine(ball.GetPos(), ball.GetRadius(), { PADDING,PADDING + (SCREEN_HEIGHT - (2 * PADDING)) }, { PADDING + (SCREEN_WIDTH - (2 * PADDING)),PADDING + (SCREEN_HEIGHT - (2 * PADDING)) }) && ball.GetState() == Bricks::BallState::Flight) {
                    ball.SetState(Bricks::BallState::Return);
                    ball.SetPrevState(Bricks::BallState::Flight);
                    Vector2 dir = { startPos.x - ball.GetPos().x, startPos.y - ball.GetPos().y };
                    dir = NormalizeVector(dir);
                    ball.SetDir(dir);
                }

                if (CheckCollisionPointCircle(startPos, ball.GetPos(), ball.GetRadius()) && (ball_state == Bricks::BallState::Flight || ball_state == Bricks::BallState::Return)) {
                    ball.SetState(Bricks::BallState::Resting);
                    ball.SetPrevState(Bricks::BallState::Return);
                    ball.SetPos(startPos);
                }

                // Collision with left vertical boundary
                if (CheckCollisionCircleLine(ball.GetPos(), ball.GetRadius(), { PADDING,PADDING }, { PADDING,PADDING + (SCREEN_HEIGHT - (2 * PADDING)) })) {
                    if (game.GetGameState().bricksALive == 0) {
                        Vector2 dir = { startPos.x - ball.GetPos().x, startPos.y - ball.GetPos().y };
                        dir = NormalizeVector(dir);
                        ball.SetDir(dir);
                    }
                    else {
                        ball.SetDir({ -ball.GetDir().x, ball.GetDir().y });
                    }
                }

                // Collision with top horizontal boundary
                if (CheckCollisionCircleLine(ball.GetPos(), ball.GetRadius(), { PADDING,PADDING }, { PADDING + (SCREEN_WIDTH - (2 * PADDING)),PADDING })) {
                    if (game.GetGameState().bricksALive == 0) {
                        Vector2 dir = { startPos.x - ball.GetPos().x, startPos.y - ball.GetPos().y };
                        dir = NormalizeVector(dir);
                        ball.SetDir(dir);
                    }
                    else {
                        ball.SetDir({ ball.GetDir().x, -ball.GetDir().y });
                    }

                }

                // Collision with right vertical boundary
                if (CheckCollisionCircleLine(ball.GetPos(), ball.GetRadius(), { PADDING + (SCREEN_WIDTH - (2 * PADDING)),PADDING }, { PADDING + (SCREEN_WIDTH - (2 * PADDING)),PADDING + (SCREEN_HEIGHT - (2 * PADDING)) })) {
                    if (game.GetGameState().bricksALive == 0) {
                        Vector2 dir = { startPos.x - ball.GetPos().x, startPos.y - ball.GetPos().y };
                        dir = NormalizeVector(dir);
                        ball.SetDir(dir);
                    }
                    else {
                        ball.SetDir({ -ball.GetDir().x, ball.GetDir().y });
                    }
                }

                
                for (auto& brick : bricks) {

                    if (brick.GetState() == Bricks::BrickState::Alive) {

                        if (CheckCollisionCircleRec(ball.GetPos(), ball.GetRadius(), { brick.GetPos().x, brick.GetPos().y, BRICK_LENGTH, BRICK_LENGTH })) {
                            // Collision with brick top horizontal side
                            if (CheckCollisionCircleLine(
                                ball.GetPos(),
                                ball.GetRadius(),
                                { brick.GetPos().x, brick.GetPos().y },
                                { brick.GetPos().x + brick.GetLength(),brick.GetPos().y }
                            )) {
                                ball.SetDir({ ball.GetDir().x, -ball.GetDir().y });
                                brick.SetHealth(brick.GetHealth() - 1);
                                if (brick.GetHealth() <= 0) {
                                    brick.SetState(Bricks::BrickState::Dead);
                                    Bricks::GameState s = game.GetGameState();
                                    s.bricksALive--;
                                    game.SetGameState(s);
                                }
                            }else
                            // Collision with brick right vertical side
                            if (CheckCollisionCircleLine(
                                ball.GetPos(),
                                ball.GetRadius(),
                                { brick.GetPos().x + brick.GetLength() , brick.GetPos().y },
                                { brick.GetPos().x + brick.GetLength() ,brick.GetPos().y + brick.GetLength() }
                            )) {
                                ball.SetDir({ -ball.GetDir().x, ball.GetDir().y });
                                brick.SetHealth(brick.GetHealth() - 1);
                                if (brick.GetHealth() <= 0) {
                                    brick.SetState(Bricks::BrickState::Dead);
                                    Bricks::GameState s = game.GetGameState();
                                    s.bricksALive--;
                                    game.SetGameState(s);
                                }
                            }else
                            // Collision with brick bottom horizontal side
                            if (CheckCollisionCircleLine(
                                ball.GetPos(),
                                ball.GetRadius(),
                                { brick.GetPos().x , brick.GetPos().y + brick.GetLength() },
                                { brick.GetPos().x + brick.GetLength(),brick.GetPos().y + brick.GetLength() }
                            )) {
                                ball.SetDir({ ball.GetDir().x, -ball.GetDir().y });
                                brick.SetHealth(brick.GetHealth() - 1);
                                if (brick.GetHealth() <= 0) {
                                    brick.SetState(Bricks::BrickState::Dead);
                                    Bricks::GameState s = game.GetGameState();
                                    s.bricksALive--;
                                    game.SetGameState(s);
                                }
                            }else
                            // Collision with brick left vertical side
                            if (CheckCollisionCircleLine(
                                ball.GetPos(),
                                ball.GetRadius(),
                                { brick.GetPos().x , brick.GetPos().y },
                                { brick.GetPos().x ,brick.GetPos().y + brick.GetLength() }
                            )) {
                                ball.SetDir({ -ball.GetDir().x, ball.GetDir().y });
                                brick.SetHealth(brick.GetHealth() - 1);

                                if (brick.GetHealth() <= 0) {
                                    brick.SetState(Bricks::BrickState::Dead);
                                    Bricks::GameState s = game.GetGameState();
                                    s.bricksALive--;
                                    game.SetGameState(s);

                                }
                            }
                        }

                    }
                }

            }
        }

        
        for (auto& ball : balls) {
            Bricks::BallState ball_state = ball.GetState();
            Bricks::BallState ball_prev_state = ball.GetPrevState();

            if (ball_state == Bricks::BallState::Resting && ball_prev_state == Bricks::BallState::Return ) {
                ball.SetPrevState(Bricks::BallState::Resting);
                Bricks::GameState s = game.GetGameState();
                s.numBallsReturned++;
                game.SetGameState(s);
            }
        }

        if (game.GetGameState().numBallsReturned >= balls.size()) {
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

        EndDrawing();
    }

    CloseWindow();
	return 0;
}
