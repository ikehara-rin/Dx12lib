// Dx12App.h
#pragma once

// 必要なWindowsヘッダーと、DirectX12用ヘッダー
#include <windows.h>
#include <wrl.h>        // ComPtrを使うため（参照カウント自動管理スマートポインタ）
#include <d3d12.h>      // DirectX12本体
#include <dxgi1_6.h>    // DXGI：グラフィックスインターフェース管理

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

private:
    // DirectX12オブジェクト管理
    Microsoft::WRL::ComPtr<ID3D12Device> device_;                   // GPUとやりとりする窓口
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;        // GPUに命令を送るためのキュー
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;              // 表示用バックバッファ
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_; // コマンドバッファメモリ確保
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;   // 描画コマンドを詰めるリスト

    // ウィンドウサイズ
    int width_;
    int height_;
};
