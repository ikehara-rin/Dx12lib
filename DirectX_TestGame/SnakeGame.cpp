#include "SnakeGame.h"
#include <cstdlib>
#include <Windows.h>

SnakeGame::SnakeGame(Renderer* renderer)
    : renderer(renderer),currentDirection(Direction::Right), moveTimer(0.0f), moveInterval(0.2f),
    gridWidth(20), gridHeight(15), cellSize(32.0f), isGameOver(false), score(0) 
{
    spriteRenderer = new SpriteRenderer(renderer);
    inputManager = new Input();
}

SnakeGame::~SnakeGame() {
    delete spriteRenderer;
    delete food;
    delete snake;
}

bool SnakeGame::Initialize() {
    if (!spriteRenderer->Initialize()) {
        OutputDebugStringA("SpriteRenderer Initialize failed\n");
        return false;
    }
    food = new Food(gridWidth,gridHeight);
    snake = new Snake(gridWidth, gridHeight, cellSize);
    Reset();
    return true;
}

//void SnakeGame::Reset() {
//    moveTimer = 0.0f;
//    isGameOver = false;
//    score = 0;
//    food->Respawn();
//}

void SnakeGame::Update(float deltaTime) {
    /*if (isGameOver) {
        if (inputManager->IsKeyPressed(VK_SPACE)) {
            Reset();
        }
        return;
    }*/

    moveTimer += deltaTime;

    inputManager->Update();

    switch (gameState) {
    case GameState::Title:
        UpdateTitle(deltaTime);
        break;
    case GameState::Playing:
        UpdatePlaying(deltaTime);
        break;
    case GameState::GameOver:
        UpdateGameOver(deltaTime);
        break;
    }

    // 入力処理
    if (inputManager->IsKeyPressed(VK_UP) && currentDirection != Direction::Down)
        currentDirection = Direction::Up;
    else if (inputManager->IsKeyPressed(VK_DOWN) && currentDirection != Direction::Up)
        currentDirection = Direction::Down;
    else if (inputManager->IsKeyPressed(VK_LEFT) && currentDirection != Direction::Right)
        currentDirection = Direction::Left;
    else if (inputManager->IsKeyPressed(VK_RIGHT) && currentDirection != Direction::Left)
        currentDirection = Direction::Right;

    

    

}

void SnakeGame::UpdateTitle(float) {
    if (inputManager->IsKeyPressed(VK_RETURN)) {
        Reset();
        gameState = GameState::Playing;
    }
}

void SnakeGame::UpdatePlaying(float deltaTime) {
    

    if (snake->CheckSelfCollision()) {
        gameState = GameState::GameOver;
        return;
    }

    if (snake->CheckCollision(food->GetX(), food->GetY())) {
        snake->Grow();
        food->Respawn();
        score++;
    }
    if (moveTimer >= moveInterval) {

        snake->Update();
        moveTimer = 0.0f;
    }
}

void SnakeGame::UpdateGameOver(float) {
    if (inputManager->IsKeyPressed(VK_RETURN)) {
        gameState = GameState::Title;
    }
}
void SnakeGame::Move() {
    //DirectX::XMFLOAT2 head = snake.front();

    //switch (currentDirection) {
    //case Direction::Up:    head.y -= 1; break;
    //case Direction::Down:  head.y += 1; break;
    //case Direction::Left:  head.x -= 1; break;
    //case Direction::Right: head.x += 1; break;
    //}

    //snake.push_front(head);

    //// 食べ物との衝突でなければ末尾を削除
    //if (static_cast<int>(head.x) == food->GetX() &&
    //    static_cast<int>(head.y) == food->GetY()) {
    //    score += 10;
    //    food->Respawn();
    //}
    //else {
    //    snake.pop_back();
    //}
}

void SnakeGame::CheckCollision() {
    //DirectX::XMFLOAT2 head = snake.front();

    //// 壁との衝突
    //if (head.x < 0 || head.y < 0 || head.x >= gridWidth || head.y >= gridHeight) {
    //    isGameOver = true;
    //    return;
    //}

    //// 自分との衝突
    //for (size_t i = 1; i < snake.size(); ++i) {
    //    if (snake[i].x == head.x && snake[i].y == head.y) {
    //        isGameOver = true;
    //        return;
    //    }
    //}
}

void SnakeGame::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList) 
{
    switch (gameState) {
    case GameState::Title:
        // TODO: タイトル画像やテキスト描画
        break;
    case GameState::Playing: {
        for (const auto& segment : snake->GetSegments()) {
            spriteRenderer->DrawCell(commandList,
            segment.x * cellSize, segment.y * cellSize, cellSize);
        }

        // DrawCell() を呼ぶ直前に
        assert(commandList->Close() == E_FAIL); // Close()済みならここで true になる
        spriteRenderer->DrawCell(commandList,
        food->GetX() * cellSize, food->GetY() * cellSize, cellSize);
        break;
    }
    case GameState::GameOver:
        // TODO: ゲームオーバー画面描画
        /*if (isGameOver) {
            DrawText(commandList, L"Game Over - SPACE to restart", 50, 50);
        }*/
        break;
    }
    
}

void SnakeGame::Reset() {
    food->Respawn();
    score = 0;
}