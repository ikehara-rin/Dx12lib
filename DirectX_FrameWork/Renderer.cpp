#include "Renderer.h"
#include <stdexcept>
#include <vector>
#include <dxgi1_6.h>
#include <d3d12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

Renderer::Renderer() : frameIndex(0), rtvDescriptorSize(0) {}

Renderer::~Renderer() {}

bool Renderer::Initialize(HWND hwnd, uint32_t width, uint32_t height) {
    if (!InitDevice()) return false;
    if (!InitCommandObjects()) return false;
    if (!InitSwapChain(hwnd, width, height)) return false;
    if (!InitRenderTargetView()) return false;
    CreateCommandAllocatorAndList();  // 追加
    return true;
}

bool Renderer::InitDevice() {
    UINT dxgiFactoryFlags = 0;

    HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
    if (FAILED(hr)) return false;

    hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
    return SUCCEEDED(hr);
}

bool Renderer::InitCommandObjects() {
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
        return false;

    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))))
        return false;

    if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
        return false;

    return true;
}

bool Renderer::InitSwapChain(HWND hwnd, uint32_t width, uint32_t height) {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
    if (FAILED(factory->CreateSwapChainForHwnd(
        commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &tempSwapChain)))
        return false;

    if (FAILED(tempSwapChain.As(&swapChain)))
        return false;

    frameIndex = swapChain->GetCurrentBackBufferIndex();
    return true;
}

bool Renderer::InitRenderTargetView() {
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap))))
        return false;

    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < FrameCount; ++i) {
        if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]))))
            return false;

        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.ptr += rtvDescriptorSize;
    }

    return true;
}

void Renderer::Render() {
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), nullptr);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = renderTargets[frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += frameIndex * rtvDescriptorSize;

    const float clearColor[] = { 0.2f, 0.4f, 0.6f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList->ResourceBarrier(1, &barrier);

    commandList->Close();

    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, ppCommandLists);
}

void Renderer::Present() {
    swapChain->Present(1, 0);
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

void Renderer::CreateCommandAllocatorAndList() {
    // コマンドアロケータを作成
    if (FAILED(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&commandAllocator)))) {
        throw std::runtime_error("CreateCommandAllocator failed");
    }

    // コマンドリストを作成
    if (FAILED(device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&commandList)))) {
        throw std::runtime_error("CreateCommandList failed");
    }

    // 最初は閉じておく（再利用可能にするため）
    commandList->Close();
}
