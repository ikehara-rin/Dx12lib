#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

class Sprite {
public:
    Sprite();
    ~Sprite();

    bool Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const wchar_t* texturePath);
    void Draw(ID3D12GraphicsCommandList* commandList);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> texture;         // テクスチャ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;    // 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;              // 頂点バッファビュー
};
