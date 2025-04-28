#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include "d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

// �O���[�o���ϐ�
HWND g_hWnd = nullptr;
ComPtr<ID3D12Device> g_device;
ComPtr<ID3D12CommandQueue> g_commandQueue;
ComPtr<IDXGISwapChain3> g_swapChain;

// �ǉ��i�R�}���h�A���P�[�^�j
ComPtr<ID3D12CommandAllocator> g_commandAllocator;
ComPtr<ID3D12GraphicsCommandList> g_commandList;
ComPtr<ID3D12DescriptorHeap> g_rtvHeap;
ComPtr<ID3D12Resource> g_renderTargets[2];
UINT g_rtvDescriptorSize;

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

    // �R�}���h�A���P�[�^�쐬
    HRESULT hr = g_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&g_commandAllocator)
    );
    assert(SUCCEEDED(hr));

    // �R�}���h���X�g�쐬
    hr = g_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        g_commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&g_commandList)
    );
    assert(SUCCEEDED(hr));

    // �����ŃR�}���h���X�g�͊J���Ă���̂ŕ���K�v����
    g_commandList->Close();

    // RTV�p�f�B�X�N���v�^�q�[�v�쐬
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 2;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        hr = g_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&g_rtvHeap));
        assert(SUCCEEDED(hr));
        g_rtvDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // �X���b�v�`�F�C���̃o�b�t�@�擾��RTV�쐬
    {
        // ��{�n���h�����擾
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_rtvHeap->GetCPUDescriptorHandleForHeapStart();

        // �C���f�b�N�X�������I�t�Z�b�g����
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