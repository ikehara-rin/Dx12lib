#pragma once

#include "Renderer.h"
#include "TextureLoader.h"
#include "Sprite.h"
#include "Input.h"
#include <vector>
#include <windows.h>

// �ȈՓI�Ȓ萔��`�i�Z���T�C�Y��}�b�v�T�C�Y�j
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

    // �X�v���C�g�p
    Sprite* snakeSprite = nullptr;
    Sprite* foodSprite = nullptr;
    Sprite* bgSprite = nullptr;

    // �Q�[�����
    GameState state = GameState::Title;
    bool running = true;
    float moveTimer = 0.0f;
    float moveInterval = 0.2f;  // �ړ��Ԋu�i�b�j

    // �X�l�[�N�̍��W�i�擪�����j
    std::vector<POINT> snake;
    POINT direction = { 1, 0 }; // �E�֐i�s
    POINT food;

    HWND hwnd;
};
