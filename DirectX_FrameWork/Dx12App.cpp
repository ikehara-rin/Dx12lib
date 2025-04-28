#include <windows.h>
#include "Renderer.h"

Renderer g_renderer;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
                      hInstance, nullptr, nullptr, nullptr, nullptr,
                      L"DX12Sample", nullptr };
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow(wc.lpszClassName, L"DirectX12 Framework",
        WS_OVERLAPPEDWINDOW, 100, 100, 800, 600,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (!g_renderer.Initialize(hWnd))
    {
        MessageBox(nullptr, L"Renderer Initialization Failed!", L"Error", MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            g_renderer.Render();
        }
    }

    g_renderer.Cleanup();
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
