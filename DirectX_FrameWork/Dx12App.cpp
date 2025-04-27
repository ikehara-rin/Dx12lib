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

    return true;
}

void Dx12App::Update()
{
    // ゲームロジックやアニメーション更新処理をここに書く
    // 今は何もなし（後で追加）
}

void Dx12App::Render()
{
    // 描画処理
    // 今はまだ画面クリアもしていない（後でRenderTargetView作成して追加予定）
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
