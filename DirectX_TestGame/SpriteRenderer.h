#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "Renderer.h"
#include <Windows.h>

class SpriteRenderer
{
public:
    SpriteRenderer(Renderer* renderer);

    bool Initialize();
    void DrawCell(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, float x, float y, float cellSize);
    void DrawSprite(
        ID3D12Resource* texture,
        float x, float y,
        float width, float height);
private:
    Renderer* m_renderer;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};

    Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
    uint8_t* m_pConstantBufferWO = nullptr;

    D3D12_GPU_DESCRIPTOR_HANDLE m_srvHandleGPU{};

    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
    };

    bool CreateRootSignature();
    bool CreatePipelineState();
    bool CreateVertexBuffer();
};
