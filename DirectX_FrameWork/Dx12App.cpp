// Dx12App.cpp
#include "Dx12App.h"
#include <stdexcept>    // 例外用（失敗時 throw 用）

Dx12App::Dx12App()
    : width_(0), height_(0)
{
}

Dx12App::~Dx12App()
{
}

bool Dx12App::Initialize(HWND hwnd, int width, int height)
{
    // ウィンドウサイズを保存
    width_ = width;
    height_ = height;

    // 各種初期化処理を順番に実行
    CreateDevice();
    CreateCommandQueue();
    CreateSwapChain(hwnd, width, height);
    CreateCommandAllocatorAndList();

    CreateRenderTargetView();
    return true;
}

// ゲームロジックやアニメーション更新処理をここに書く
void Dx12App::Update()
{
    
    // 今は何もなし（後で追加）
}

// 描画処理
void Dx12App::Render()
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

void Dx12App::Finalize()
{
    // ComPtrなので自動で解放されるが、明示的にリセットもできる
    commandList_.Reset();
    commandAllocator_.Reset();
    swapChain_.Reset();
    commandQueue_.Reset();
    device_.Reset();
}

// ==========================
// 以下、内部初期化関数
// ==========================

void Dx12App::CreateDevice()
{
    // DXGIファクトリー作成（GPU情報を取得したり、デバイス作成準備）
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    // GPUアダプター取得（今回は0番＝一番良いGPUを取る）
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    factory->EnumAdapters1(0, &adapter);

    // DirectX12デバイス作成（GPUに命令を送るためのメインオブジェクト）
    D3D12CreateDevice(
        adapter.Get(),
        D3D_FEATURE_LEVEL_11_0,   // 必要最低限の機能レベル（12_0でもOK）
        IID_PPV_ARGS(&device_)
    );
}

void Dx12App::CreateCommandQueue()
{
    // コマンドキュー設定
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;  // 直接描画コマンドを送れる種類

    // コマンドキュー作成
    device_->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue_));
}

void Dx12App::CreateSwapChain(HWND hwnd, int width, int height)
{
    // スワップチェイン＝画面に絵を表示するためのバッファ
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;                       // ダブルバッファリング
    swapChainDesc.Width = width;                         // ウィンドウ幅
    swapChainDesc.Height = height;                       // ウィンドウ高さ
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // カラーフォーマット（RGBA8bit）
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画用
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // モダンなページフリップ方式
    swapChainDesc.SampleDesc.Count = 1;                  // マルチサンプリングなし

    // スワップチェイン作成（ウィンドウに接続）
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChainTemp;
    factory->CreateSwapChainForHwnd(
        commandQueue_.Get(),     // コマンドキューと関連付け
        hwnd,                    // ウィンドウハンドル
        &swapChainDesc,           // 設定情報
        nullptr,                  // フルスクリーン設定（今はウィンドウ）
        nullptr,                  // 出力先モニタ（自動選択）
        &swapChainTemp            // 返り値
    );

    // IDXGISwapChain4にキャスト
    swapChainTemp.As(&swapChain_);
}

void Dx12App::CreateCommandAllocatorAndList()
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

void Dx12App::CreateRenderTargetView()
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