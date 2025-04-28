// Dx12App.cpp
#include "Dx12App.h"
#include "Renderer.h"
#include <stdexcept>    // 例外用（失敗時 throw 用）

Dx12App::Dx12App()
    : width_(0), height_(0), renderer_(nullptr)
{
}

Dx12App::~Dx12App()
{
    if (renderer_)
    {
        delete renderer_;
    }
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

    // Rendererを初期化
    renderer_ = new Renderer(device_.Get(), commandQueue_.Get(), swapChain_.Get());
    renderer_->Initialize();

    return true;
}

void Dx12App::Update()
{
    // ゲームロジックやアニメーション更新処理（後で追加）
}

void Dx12App::Render()
{
    // レンダリング処理
    renderer_->Render();
}

void Dx12App::Finalize()
{
    // リソース解放
    renderer_->Finalize();
    delete renderer_;
    renderer_ = nullptr;
}

// ==========================
// 以下、内部初期化関数
// ==========================
