#include <windows.h>
#include "SnakeGame.h"

int main() {
    // DirectX12�̏���������
    HWND hwnd = CreateWindowEx(0, L"STATIC", L"Snake Game", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, nullptr, nullptr);

    // �Q�[���I�u�W�F�N�g�̏�����
    SnakeGame game(hwnd);

    // �Q�[���J�n
    game.Run();

    return 0;
}
