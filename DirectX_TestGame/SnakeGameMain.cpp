#include <windows.h>
#include "SnakeGame.h"

int main() {
    // DirectX12の初期化処理
    HWND hwnd = CreateWindowEx(0, L"STATIC", L"Snake Game", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, nullptr, nullptr);

    // ゲームオブジェクトの初期化
    SnakeGame game(hwnd);

    // ゲーム開始
    game.Run();

    return 0;
}
