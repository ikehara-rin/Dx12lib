// main.cpp
#include <windows.h>
#include "Renderer.h"
#include "SnakeGame.h"

// グローバル変数
HWND g_hWnd = nullptr;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND InitWindow(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0L, 0L,
        hInstance,
        nullptr, nullptr, nullptr, nullptr,
        L"SnakeWindowClass",
        nullptr
    };

    RegisterClassEx(&wc);

    RECT wr = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindow(
        wc.lpszClassName,
        L"Snake Game",
        WS_OVERLAPPEDWINDOW,
        100, 100,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr, nullptr,
        wc.hInstance,
        nullptr
    );

    ShowWindow(hWnd, nCmdShow);
    return hWnd;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    g_hWnd = InitWindow(hInstance, nCmdShow);

    // DirectX12 Renderer の初期化
    Renderer renderer;
    if (!renderer.Initialize(g_hWnd, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        MessageBox(nullptr, L"Renderer Initialization Failed", L"Error", MB_OK);
        return -1;
    }

    // SnakeGame 初期化
    SnakeGame game(&renderer);
    if (!game.Initialize()) {
        MessageBox(nullptr, L"Game Initialization Failed", L"Error", MB_OK);
        return -1;
    }

    // タイマー
    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // タイマー更新
        QueryPerformanceCounter(&now);
        float deltaTime = static_cast<float>(now.QuadPart - start.QuadPart) / freq.QuadPart;
        start = now;

        // ゲーム更新
        game.Update(deltaTime);

        // 描画
        renderer.BeginFrame();
        game.Render(renderer.GetCommandList());
        renderer.EndFrame();
    }

    UnregisterClass(L"SnakeWindowClass", hInstance);
    return 0;
}
