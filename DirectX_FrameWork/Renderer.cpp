// Renderer.cpp
#include "Renderer.h"
#include <stdexcept>    // 例外用（失敗時 throw 用）

Renderer::Renderer(ID3D12Device* device, ID3D12CommandQueue* commandQueue, IDXGISwapChain4* swapChain)
    : device_(device), commandQueue_(commandQueue), swapChain_(swapChain), rtvDescriptorSize_(0)
{
}

Renderer::~Renderer()
{
}

void Renderer::Initialize()
{
    CreateCommandAllocatorAndList();
    CreateRenderTargetView();
}

void Renderer::Render()
{
    // コマンドリストをリセット
    commandAllocator_->Reset();
    commandList_->Reset(commandAllocator_.Get(), nullptr);

    // 現在のバックバッファインデックスを取得
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    // バックバッファのRTVハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += backBufferIndex * rtvDescriptorSize_;

    // リソースバリア（Present→RenderTarget）
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = renderTargets_[backBufferIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList_->ResourceBarrier(1, &barrier);

    // レンダーターゲットを設定
    commandList_->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // 画面をクリア（背景色を設定）
    FLOAT clearColor[] = { 0.2f, 0.4f, 0.6f, 1.0f }; // RGBA（ここを変えると色が変わる）
    commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // リソースバリア（RenderTarget→Present）
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList_->ResourceBarrier(1, &barrier);

    // コマンドリストをCloseしてGPUに送る準備
    commandList_->Close();

    // コマンドリストを実行
    ID3D12CommandList* cmdLists[] = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, cmdLists);

    // バックバッファを前面に表示
    swapChain_->Present(1, 0);
}

void Renderer::Finalize()
{
    // ComPtrなので自動で解放されるが、明示的にリセットもできる
    commandList_.Reset();
    commandAllocator_.Reset();
    swapChain_.Reset();
    commandQueue_.Reset();
    device_.Reset();
}

void Renderer::CreateCommandAllocatorAndList()
{
    // コマンドアロケータ＝コマンドリストが使うメモリ確保
    device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));

    // コマンドリスト＝描画命令を溜めるバッファ
    device_->CreateCommandList(
        0,                              // ノードID（シングルGPUなら0）
        D3D12_COMMAND_LIST_TYPE_DIRECT, // 描画用コマンドリスト
        commandAllocator_.Get(),         // アロケータを指定
        nullptr,                         // 最初にバインドするパイプラインステート（今はなし）
        IID_PPV_ARGS(&commandList_)
    );

    // コマンドリストは作成直後は「オープン状態」なので一旦Closeする
    commandList_->Close();
}

void Renderer::CreateRenderTargetView()
{
    // ★バッファ数はSwapChain作成時に2個にしたので2
    const UINT bufferCount = 2;

    // ★RTV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = bufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap_));

    // ★ディスクリプタ1個あたりのサイズを取得
    rtvDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // ★バックバッファ（描画対象）のリソースを取得する
    renderTargets_.resize(bufferCount);
    for (UINT i = 0; i < bufferCount; ++i)
    {
        swapChain_->GetBuffer(i, IID_PPV_ARGS(&renderTargets_[i]));

        // ★RTVを作成してディスクリプタヒープに登録する
        D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += i * rtvDescriptorSize_;  // i番目の場所に

        device_->CreateRenderTargetView(renderTargets_[i].Get(), nullptr, handle);
    }
}
