#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize(HWND hwnd, uint32_t width, uint32_t height);
    void Render();
    void Present();
    void BeginFrame();
    void EndFrame();

    // Getä÷êîåQ
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device; }
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return commandQueue; }
    Microsoft::WRL::ComPtr<IDXGISwapChain4> GetSwapChain() const { return swapChain; }
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return rtvHeap; }
    UINT GetRTVDescriptorSize() const { return rtvDescriptorSize; }
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList; }
    ID3D12DescriptorHeap* GetSRVHeap() const;
    ID3D12PipelineState* GetPipelineState() const { return pipelineState.Get(); }
    ID3D12RootSignature* GetRootSignature() const { return rootSignature.Get(); }

private:
    bool InitDevice();
    bool InitCommandObjects();
    bool InitSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    bool InitRenderTargetView();
    void CreateCommandAllocatorAndList();

private:
    HWND hwnd;

    static const uint32_t FrameCount = 2;

    Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[FrameCount];
    UINT rtvDescriptorSize;

    UINT frameIndex;

    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    uint64_t fenceValue = 0;
    HANDLE fenceEvent = nullptr;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
    UINT srvDescriptorSize = 0;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    bool CreatePipelineStateObjects();
};
