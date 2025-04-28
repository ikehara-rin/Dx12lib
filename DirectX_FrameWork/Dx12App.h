// Dx12App.h
#pragma once
#ifndef DX12APP_H
#define DX12APP_H

#include <windows.h>
#include "Renderer.h"
#include <d3d12.h>
#include <dxgi1_6.h>

class Dx12App
{
public:
    Dx12App();
    ~Dx12App();

    bool Initialize(HWND hwnd, int width, int height);
    void Update();
    void Render();
    void Finalize();

private:
    int width_;
    int height_;

    // DirectX 12のメンバ
    Microsoft::WRL::ComPtr<ID3D12Device> device_;  // ここでdevice_を定義
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;

    Renderer* renderer_;  // Rendererインスタンスを保持

    // 各種初期化関数
    void CreateDevice();
    void CreateCommandQueue();
    void CreateSwapChain(HWND hwnd, int width, int height);
};

#endif // DX12APP_H
