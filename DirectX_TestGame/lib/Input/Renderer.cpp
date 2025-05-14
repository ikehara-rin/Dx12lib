#include "Renderer.h"
#include <stdexcept>
#include <cassert>

Renderer::Renderer(HWND hwnd, uint32_t width, uint32_t height) {
    Initialize();
    CreateDevice();
    CreateCommandQueue();
    CreateSwapChain(hwnd, width, height);
    CreateDescriptorHeap();
    CreateRenderTargetViews();
    CreateCommandAllocatorsAndList();
    CreateFence();
}

Renderer::~Renderer() {
    WaitForGPU();
    CloseHandle(fenceEvent_);
}

void Renderer::Initialize() {
    // Debug layer (è»ó™â¬î\)
#if defined(_DEBUG)
    Microsoft::WRL::ComPtr<ID3D12Debug> debug;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
        debug->EnableDebugLayer();
    }
#endif
}

void Renderer::CreateDevice() {
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); ++i) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_)))) {
            break;
        }
    }
    assert(device_ && "Failed to create D3D12 device.");
}

void Renderer::CreateCommandQueue() {
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    device_->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue_));
}

void Renderer::CreateSwapChain(HWND hwnd, uint32_t width, uint32_t height) {
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.BufferCount = FrameCount;
    scDesc.Width = width;
    scDesc.Height = height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
    factory->CreateSwapChainForHwnd(commandQueue_.Get(), hwnd, &scDesc, nullptr, nullptr, &swapChain);
    swapChain.As(&swapChain_);
    frameIndex_ = swapChain_->GetCurrentBackBufferIndex();
}

void Renderer::CreateDescriptorHeap() {
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = FrameCount;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    device_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap_));
    rtvDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void Renderer::CreateRenderTargetViews() {
    D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < FrameCount; ++i) {
        swapChain_->GetBuffer(i, IID_PPV_ARGS(&renderTargets_[i]));
        device_->CreateRenderTargetView(renderTargets_[i].Get(), nullptr, handle);
        handle.ptr += rtvDescriptorSize_;
    }
}

void Renderer::CreateCommandAllocatorsAndList() {
    for (UINT i = 0; i < FrameCount; ++i) {
        device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators_[i]));
    }

    device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators_[frameIndex_].Get(), nullptr, IID_PPV_ARGS(&commandList_));
    commandList_->Close();
}

void Renderer::CreateFence() {
    device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(fenceEvent_);
}

void Renderer::WaitForGPU() {
    const UINT64 fenceValue = ++fenceValues_[frameIndex_];
    commandQueue_->Signal(fence_.Get(), fenceValue);

    if (fence_->GetCompletedValue() < fenceValue) {
        fence_->SetEventOnCompletion(fenceValue, fenceEvent_);
        WaitForSingleObject(fenceEvent_, INFINITE);
    }

    frameIndex_ = swapChain_->GetCurrentBackBufferIndex();
}

void Renderer::Render() {
    // Reset
    commandAllocators_[frameIndex_]->Reset();
    commandList_->Reset(commandAllocators_[frameIndex_].Get(), nullptr);

    // Transition
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = renderTargets_[frameIndex_].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList_->ResourceBarrier(1, &barrier);

    // Clear
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += frameIndex_ * rtvDescriptorSize_;
    const float clearColor[] = { 0.2f, 0.3f, 0.4f, 1.0f };
    commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // Transition back
    std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);
    commandList_->ResourceBarrier(1, &barrier);

    commandList_->Close();

    // Execute
    ID3D12CommandList* cmdLists[] = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    // Present
    swapChain_->Present(1, 0);

    WaitForGPU();
}
