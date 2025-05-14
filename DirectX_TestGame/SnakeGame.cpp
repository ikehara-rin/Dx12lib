#include "SnakeGame.h"
#include <Windows.h>
#include <cstdlib>
#include <ctime>

extern const int cellSize = 32;
extern ID3D12Resource* snakeTexture;
extern ID3D12Resource* foodTexture;

SnakeGame::SnakeGame(int fieldWidth, int fieldHeight, int cellSize)
    : fieldWidth(fieldWidth), fieldHeight(fieldHeight),
    moveTimer(0.0f), moveInterval(0.2f), grow(false)
{
    snake.push_back({ fieldWidth / 2, fieldHeight / 2 });
    direction = { 1, 0 };

    food = new Food(fieldWidth, fieldHeight);  // �t�[�h����
}

SnakeGame::~SnakeGame() {
    delete food;
}

void SnakeGame::Initialize(HWND hwnd) {
    renderer.Initialize();
    spriteRenderer.Initialize(&renderer);

    snake = new Snake(fieldWidth, fieldHeight);
}

void SnakeGame::Update(float deltaTime) {
    HandleInput();
    moveTimer += deltaTime;
    if (moveTimer >= moveInterval) {
        moveTimer -= moveInterval;
        if (gameOver) return;

        snake.Move();

        // �ǂƂ̏Փ�
        int headX = snake.GetHeadX();
        int headY = snake.GetHeadY();
        if (headX < 0 || headX >= fieldWidth || headY < 0 || headY >= fieldHeight) {
            gameOver = true;
            return;
        }

        // ���ȏՓ�
        if (snake.CheckSelfCollision()) {
            gameOver = true;
            return;
        }

        // �H�ו��Ƃ̏Փ�
        if (headX == food.GetX() && headY == food.GetY()) {
            snake.Grow();
            food.Respawn();
        }
    }
}

void SnakeGame::Move() {
    DirectX::XMINT2 newHead = {
        snake[0].x + direction.x,
        snake[0].y + direction.y
    };
    snake.insert(snake.begin(), newHead);

    if (!grow) {
        snake.pop_back();
    }
    else {
        grow = false;
    }
}

void SnakeGame::CheckCollision() {
    // �ǂƂ̏Փ˃`�F�b�N
    if (snake[0].x < 0 || snake[0].x >= fieldWidth ||
        snake[0].y < 0 || snake[0].y >= fieldHeight) {
        // ���Z�b�g
        snake.clear();
        snake.push_back({ fieldWidth / 2, fieldHeight / 2 });
        direction = { 1, 0 };
    }

    // �����Ƃ̏Փ˃`�F�b�N
    for (size_t i = 1; i < snake.size(); ++i) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            // ���Z�b�g
            snake.clear();
            snake.push_back({ fieldWidth / 2, fieldHeight / 2 });
            direction = { 1, 0 };
            break;
        }
    }

    // �t�[�h�Ƃ̓����蔻��
    if (snake[0].x == food->GetX() &&
        snake[0].y == food->GetY()) {
        grow = true;
        food->Respawn();  // �t�[�h�Đ���
    }
}

void SnakeGame::Render(SpriteRenderer& renderer) {
    renderer.Clear();
    DrawSnake(renderer);
    DrawFood(renderer);
    spriteRenderer.Render();
    renderer.Present();
}

void SnakeGame::HandleInput() {
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        snake->ChangeDirection(Direction::Up);
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        snake->ChangeDirection(Direction::Down);
    }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        snake->ChangeDirection(Direction::Left);
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        snake->ChangeDirection(Direction::Right);
    }
}

void SnakeGame::DrawSnake(SpriteRenderer& renderer) {
    ID3D12GraphicsCommandList* commandList = renderer->GetCommandList();
    for (const auto& segment : snake->GetSegments()) {
        spriteRenderer.DrawCell(commandList, snakeTexture, segment.x, segment.y,cellSize); // textureIndex��0�����g�p
    }
}

void SnakeGame::DrawFood(SpriteRenderer& renderer) {
    ID3D12GraphicsCommandList* commandList = renderer->GetCommandList();
    const auto& pos = food->GetPosition();
    renderer.DrawCell(commandList, snakeTexture,pos.x, pos.y, cellSize ); // �Ԃ��t�[�h
}