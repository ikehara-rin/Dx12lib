#include <windows.h>

// グローバル変数
HWND g_hWnd = nullptr;

// ウィンドウプロシージャ（イベント処理）
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0); // 終了メッセージ
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// エントリーポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // ウィンドウクラスの登録
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DX12SampleWindowClass";
    RegisterClassEx(&wc);

    // ウィンドウ作成
    g_hWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"DirectX12 Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1280, 720,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // メッセージループ
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // --- ここにDirectX12のレンダリング処理を後で入れる ---
        }
    }

    return static_cast<int>(msg.wParam);
}