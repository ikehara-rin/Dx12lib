#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include "d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

// グローバル変数
HWND g_hWnd = nullptr;
ComPtr<ID3D12Device> g_device;
ComPtr<ID3D12CommandQueue> g_commandQueue;
ComPtr<IDXGISwapChain3> g_swapChain;

// 追加（コマンドアロケータ）
ComPtr<ID3D12CommandAllocator> g_commandAllocator;
ComPtr<ID3D12GraphicsCommandList> g_commandList;
ComPtr<ID3D12DescriptorHeap> g_rtvHeap;
ComPtr<ID3D12Resource> g_renderTargets[2];
UINT g_rtvDescriptorSize;

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// DirectX12初期化処理
void InitD3D12()
{
    // デバイス作成
    {
        ComPtr<IDXGIFactory6> factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        ComPtr<IDXGIAdapter1> adapter;
        factory->EnumAdapters1(0, &adapter);

        HRESULT hr = D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL_11_0, // 最低限動かすための機能レベル
            IID_PPV_ARGS(&g_device)
        );
        assert(SUCCEEDED(hr)); // 作成失敗したら即止める
    }

    // コマンドキュー作成
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // 基本的な命令を流すタイプ

        HRESULT hr = g_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_commandQueue));
        assert(SUCCEEDED(hr));
    }

    // スワップチェイン作成
    {
        ComPtr<IDXGIFactory6> factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        DXGI_SWAP_CHAIN_DESC1 scDesc = {};
        scDesc.BufferCount = 2; // ダブルバッファ
        scDesc.Width = 1280;
        scDesc.Height = 720;
        scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bitカラーフォーマット
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        scDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain1;
        HRESULT hr = factory->CreateSwapChainForHwnd(
            g_commandQueue.Get(),
            g_hWnd,
            &scDesc,
            nullptr,
            nullptr,
            &swapChain1
        );
        assert(SUCCEEDED(hr));

        swapChain1.As(&g_swapChain); // バージョンアップ
    }

    // コマンドアロケータ作成
    HRESULT hr = g_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&g_commandAllocator)
    );
    assert(SUCCEEDED(hr));

    // コマンドリスト作成
    hr = g_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        g_commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&g_commandList)
    );
    assert(SUCCEEDED(hr));

    // ここでコマンドリストは開いているので閉じる必要あり
    g_commandList->Close();

    // RTV用ディスクリプタヒープ作成
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 2;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        hr = g_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&g_rtvHeap));
        assert(SUCCEEDED(hr));
        g_rtvDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // スワップチェインのバッファ取得＆RTV作成
    {
        // 基本ハンドルを取得
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_rtvHeap->GetCPUDescriptorHandleForHeapStart();

        // インデックス分だけオフセットする
        rtvHandle.ptr += g_swapChain->GetCurrentBackBufferIndex() * g_rtvDescriptorSize;

        for (UINT i = 0; i < 2; ++i)
        {
            hr = g_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_renderTargets[i]));
            assert(SUCCEEDED(hr));

            g_device->CreateRenderTargetView(g_renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, g_rtvDescriptorSize);
        }
    }

}

// エントリーポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // ウィンドウクラス登録
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

    // DirectX12初期化
    InitD3D12();

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
            // ここでレンダリング処理を後で書く
        }
    }

    return static_cast<int>(msg.wParam);
}