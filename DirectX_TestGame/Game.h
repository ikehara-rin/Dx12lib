#pragma once

#include "Renderer.h"
#include "TextureLoader.h"
#include "Sprite.h"
#include "Input.h"
#include <vector>
#include <windows.h>

// 簡易的な定数定義（セルサイズやマップサイズ）
constexpr int CELL_SIZE = 32;
constexpr int MAP_WIDTH = 20;
constexpr int MAP_HEIGHT = 15;

enum class GameState {
    Title,
    Playing,
    GameOver
};

class Game {
public:
    Game(HWND hwnd);
    ~Game();

    bool Init();
    void Update();
    void Render();
    bool IsRunning() const { return running; }

private:
    void ResetGame();
    void UpdateSnake();
    void CheckCollision();
    void SpawnFood();

    Renderer renderer;
    TextureLoader textureLoader;
    Input input;

    // スプライト用
    Sprite* snakeSprite = nullptr;
    Sprite* foodSprite = nullptr;
    Sprite* bgSprite = nullptr;

    // ゲーム状態
    GameState state = GameState::Title;
    bool running = true;
    float moveTimer = 0.0f;
    float moveInterval = 0.2f;  // 移動間隔（秒）

    // スネークの座標（先頭が頭）
    std::vector<POINT> snake;
    POINT direction = { 1, 0 }; // 右へ進行
    POINT food;

    HWND hwnd;
};
