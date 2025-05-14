#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>

class Renderer {
public:
    Renderer(HWND hwnd, uint32_t width, uint32_t height);
    ~Renderer();

    void Initialize();
    void Render();

private:
    void CreateDevice();
    void CreateCommandQueue();
    void CreateSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    void CreateDescriptorHeap();
    void CreateRenderTargetViews();
    void CreateCommandAllocatorsAndList();
    void CreateFence();

    void WaitForGPU();

private:
    static const uint32_t FrameCount = 2;

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets_[FrameCount];
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators_[FrameCount];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    HANDLE fenceEvent_;
    UINT64 fenceValues_[FrameCount] = {};

    uint32_t rtvDescriptorSize_ = 0;
    uint32_t frameIndex_ = 0;
};
