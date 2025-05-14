#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>

class Sprite {
public:
    Sprite(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    ~Sprite();

    bool Initialize();
    void Draw();

private:
    struct Vertex {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
    };

    ID3D12Device* m_device;
    ID3D12GraphicsCommandList* m_cmdList;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vbView{};
};
