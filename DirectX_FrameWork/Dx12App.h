// Dx12App.h
#pragma once

// 必要なWindowsヘッダーと、DirectX12用ヘッダー
#include <windows.h>
#include <wrl.h>        // ComPtrを使うため（参照カウント自動管理スマートポインタ）
#include <d3d12.h>      // DirectX12本体
#include <dxgi1_6.h>    // DXGI：グラフィックスインターフェース管理
// 追加（画面クリア)
#include <d3dcompiler.h>  // これは後でシェーダー用（今は不要だけど将来のため）
#include <DirectXMath.h>  // 数学ライブラリ（これも後で便利）

#include <vector> // バックバッファを2つ管理するため

// DirectX12基本アプリケーションクラス
class Dx12App
{
public:
    Dx12App();
    ~Dx12App();

    // 初期化処理
    bool Initialize(HWND hwnd, int width, int height);

    // 毎フレーム更新処理
    void Update();

    // 毎フレーム描画処理
    void Render();

    // 終了処理（リソース解放）
    void Finalize();

private:
    // 内部的な初期化関数（各ステップを分離）
    void CreateDevice();                  // デバイス作成
    void CreateCommandQueue();             // コマンドキュー作成
    void CreateSwapChain(HWND hwnd, int width, int height);  // スワップチェイン作成
    void CreateCommandAllocatorAndList();  // コマンドアロケータ＋コマンドリスト作成
    void CreateRenderTargetView();

private:
    // DirectX12オブジェクト管理
    Microsoft::WRL::ComPtr<ID3D12Device> device_;                   // GPUとやりとりする窓口
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;        // GPUに命令を送るためのキュー
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;              // 表示用バックバッファ
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_; // コマンドバッファメモリ確保
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;   // 描画コマンドを詰めるリスト
    // 追加（画面クリア）
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_; // RTV用ヒープ
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargets_; // バックバッファ
    UINT rtvDescriptorSize_; // ヒープ内1要素あたりのサイズ

    // ウィンドウサイズ
    int width_;
    int height_;
};
