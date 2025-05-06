#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>


using Microsoft::WRL::ComPtr;

// �����_���[�N���X�iDirectX12�̏������E�`��Ǘ��j
class Renderer
{
public:
    Renderer();
    ~Renderer();

    bool Initialize(HWND hWnd);  // DirectX12������
    void BeginRender();
    void EndRender();
    void Render();               // �t���[���`��
    void Release();              // ���\�[�X�J��

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
