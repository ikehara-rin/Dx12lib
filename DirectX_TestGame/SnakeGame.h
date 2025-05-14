#pragma once
#include "Renderer.h"
#include "SpriteRenderer.h"
#include "Snake.h"
#include "Direction.h"
#include "Food.h"

class SnakeGame {
public:
    SnakeGame(int fieldWidth, int fieldHeight, int cellSize);
    ~SnakeGame();

    void Initialize(HWND hwnd);
    void Update(float deltaTime);
    void Render(SpriteRenderer& renderer);

private:
    void Move();
    void CheckCollision();
    void HandleInput();
    void DrawSnake(SpriteRenderer& renderer);
    void DrawFood(SpriteRenderer& renderer);

    Renderer renderer;
    SpriteRenderer spriteRenderer;
    Snake* snake = nullptr;

    std::vector<DirectX::XMINT2> snake;
    DirectX::XMINT2 direction;
    float moveTimer;
    float moveInterval;

    bool gameOver = false;
    const int fieldWidth = 25;
    const int fieldHeight = 18;

    Food* food;
    bool grow;
};
