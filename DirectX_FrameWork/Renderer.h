// Renderer.h
#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

class Renderer
{
public:
    Renderer(ID3D12Device* device, ID3D12CommandQueue* commandQueue, IDXGISwapChain4* swapChain);
    ~Renderer();

    void Initialize();
    void Render();
    void Finalize();

private:
    void CreateRenderTargetView();
    void CreateCommandAllocatorAndList();

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;

    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargets_;
    UINT rtvDescriptorSize_;
};

#endif // RENDERER_H
