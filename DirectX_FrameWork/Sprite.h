#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include "Renderer.h"

class Sprite {
public:
    Sprite();
    ~Sprite();

    bool Load(Renderer* renderer, const std::wstring& filePath);
    void Draw(float x, float y);

private:
    Renderer* renderer;

    Microsoft::WRL::ComPtr<ID3D12Resource> texture;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vbView{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle{};

    UINT textureWidth;
    UINT textureHeight;

    bool CreateVertexBuffer();
    bool CreateTextureFromFile(const std::wstring& filePath);
};
