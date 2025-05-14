#include "SpriteRenderer.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <stdexcept>
#include <vector>

using namespace Microsoft::WRL;
using namespace DirectX;

SpriteRenderer::SpriteRenderer()
    : m_pConstantBufferWO(nullptr) {
}

void SpriteRenderer::Initialize(ID3D12Device* device, ID3D12DescriptorHeap* srvHeap, UINT descriptorSize)
{
    struct Vertex { XMFLOAT3 pos; XMFLOAT2 uv; };
    Vertex vertices[] =
    {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},
    };

    const UINT vertexBufferSize = sizeof(vertices);

    // ヒーププロパティ初期化（UPLOADヒープ）
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    // バッファリソースディスクリプタ
    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Alignment = 0;
    bufferDesc.Width = vertexBufferSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.SampleDesc.Quality = 0;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // 頂点バッファ作成
    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)
    );
    if (FAILED(hr)) throw std::runtime_error("Vertex buffer creation failed");

    // マッピングとコピー
    UINT8* pVertexDataBegin;
    D3D12_RANGE readRange = { 0, 0 };
    m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices, vertexBufferSize);
    m_vertexBuffer->Unmap(0, nullptr);

    // 頂点バッファビュー作成
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;

    // 定数バッファ作成
    const UINT cbSize = (sizeof(XMMATRIX) + 255) & ~255;
    bufferDesc.Width = cbSize;

    hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer)
    );
    if (FAILED(hr)) throw std::runtime_error("Constant buffer creation failed");

    m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstantBufferWO));
    ZeroMemory(m_pConstantBufferWO, cbSize);

    // SRVディスクリプタハンドル
    m_srvHandleGPU = srvHeap->GetGPUDescriptorHandleForHeapStart();
}

void SpriteRenderer::DrawCell(ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* texture,
    float x, float y,
    float cellSize)
{
    if (!commandList || !texture) return;

    // SRV設定（ルートパラメータ0）
    commandList->SetGraphicsRootDescriptorTable(0, m_srvHandleGPU);

    // ワールド行列（拡大＋平行移動）
    XMMATRIX world = XMMatrixScaling(cellSize, cellSize, 1.0f) *
        XMMatrixTranslation(x, y, 0.0f);

    memcpy(m_pConstantBufferWO, &world, sizeof(world));
    commandList->SetGraphicsRootConstantBufferView(1, m_constantBuffer->GetGPUVirtualAddress());

    // 描画
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    commandList->DrawInstanced(4, 1, 0, 0);
}
