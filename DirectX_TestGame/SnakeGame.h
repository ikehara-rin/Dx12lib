#pragma once

#include "SpriteRenderer.h"
#include "InputManager.h"
#include "Snake.h"
#include "Food.h"
#include <deque>

enum class GameState {
    Title,
    Playing,
    GameOver
};

class SnakeGame {
public:
    SnakeGame(Renderer* renderer);
    ~SnakeGame();
    bool Initialize();
    void Update(float deltaTime);
    void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

private:
    enum class Direction {
        Up,
        Down,
        Left,
        Right
    };

    void Move();
    void CheckCollision();
    void Reset();

    Renderer* renderer;
    SpriteRenderer* spriteRenderer = nullptr;
    Input* inputManager = nullptr;
    Snake* snake;
    Food* food;

    GameState gameState;

    /*std::deque<DirectX::XMFLOAT2> snake;*/
    Direction currentDirection;
    float moveTimer;
    float moveInterval;

    int gridWidth;
    int gridHeight;
    float cellSize;

    bool isGameOver;
    int score;

    void UpdateTitle(float deltaTime);
    void UpdatePlaying(float deltaTime);
    void UpdateGameOver(float deltaTime);
};
