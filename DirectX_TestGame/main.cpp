#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

// �O���[�o���ϐ�
HWND g_hWnd = nullptr;
ComPtr<ID3D12Device> g_device;
ComPtr<ID3D12CommandQueue> g_commandQueue;
ComPtr<IDXGISwapChain3> g_swapChain;

// �E�B���h�E�v���V�[�W��
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

// DirectX12����������
void InitD3D12()
{
    // �f�o�C�X�쐬
    {
        ComPtr<IDXGIFactory6> factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        ComPtr<IDXGIAdapter1> adapter;
        factory->EnumAdapters1(0, &adapter);

        HRESULT hr = D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL_11_0, // �Œ�����������߂̋@�\���x��
            IID_PPV_ARGS(&g_device)
        );
        assert(SUCCEEDED(hr)); // �쐬���s�����瑦�~�߂�
    }

    // �R�}���h�L���[�쐬
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // ��{�I�Ȗ��߂𗬂��^�C�v

        HRESULT hr = g_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_commandQueue));
        assert(SUCCEEDED(hr));
    }

    // �X���b�v�`�F�C���쐬
    {
        ComPtr<IDXGIFactory6> factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        DXGI_SWAP_CHAIN_DESC1 scDesc = {};
        scDesc.BufferCount = 2; // �_�u���o�b�t�@
        scDesc.Width = 1280;
        scDesc.Height = 720;
        scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bit�J���[�t�H�[�}�b�g
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

        swapChain1.As(&g_swapChain); // �o�[�W�����A�b�v
    }
}

// �G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // �E�B���h�E�N���X�o�^
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DX12SampleWindowClass";
    RegisterClassEx(&wc);

    // �E�B���h�E�쐬
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

    // DirectX12������
    InitD3D12();

    // ���b�Z�[�W���[�v
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
            // �����Ń����_�����O��������ŏ���
        }
    }

    return static_cast<int>(msg.wParam);
}