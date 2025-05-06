#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>


using Microsoft::WRL::ComPtr;

// レンダラークラス（DirectX12の初期化・描画管理）
class Renderer
{
public:
    Renderer();
    ~Renderer();

    bool Initialize(HWND hWnd);  // DirectX12初期化
    void BeginRender();
    void EndRender();
    void Render();               // フレーム描画
    void Release();              // リソース開放

private:
    bool InitD3D12(HWND hWnd);
    bool CreateCommandObjects();
    bool CreateSwapChain(HWND hwnd);
    bool CreateRnderTargetViews();

private:
    static const UINT FrameCount = 2;

    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12Resource> m_renderTargets[2];
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12Fence> m_fence;
    HANDLE m_fenceEvent;
    UINT64 m_fenceValue;

    UINT m_rtvDescriptorSize;
    UINT m_frameIndex;
};
