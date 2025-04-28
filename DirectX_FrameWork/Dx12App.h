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

    // DirectX 12�̃����o
    Microsoft::WRL::ComPtr<ID3D12Device> device_;  // ������device_���`
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;

    Renderer* renderer_;  // Renderer�C���X�^���X��ێ�

    // �e�평�����֐�
    void CreateDevice();
    void CreateCommandQueue();
    void CreateSwapChain(HWND hwnd, int width, int height);
};

#endif // DX12APP_H
