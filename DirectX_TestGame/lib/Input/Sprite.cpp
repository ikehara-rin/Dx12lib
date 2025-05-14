#include "Sprite.h"
#include <stdexcept>

Sprite::Sprite(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
    : m_device(device), m_cmdList(cmdList) {
}

Sprite::~Sprite() {}

bool Sprite::Initialize() {
    // 簡単な四角形のスプライト（TriangleStrip）
    Vertex vertices[] = {
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f} },
        { {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f} },
        { { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f} },
        { { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f} },
    };

    const UINT vbSize = sizeof(vertices);

    D3D12_HEAP_PROPERTIES heapProp = {};
    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = vbSize;
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = m_device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)
    );

    if (FAILED(hr)) return false;

    // データ転送
    void* mapped = nullptr;
    D3D12_RANGE range = { 0, 0 };
    m_vertexBuffer->Map(0, &range, &mapped);
    memcpy(mapped, vertices, vbSize);
    m_vertexBuffer->Unmap(0, nullptr);

    m_vbView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vbView.StrideInBytes = sizeof(Vertex);
    m_vbView.SizeInBytes = vbSize;

    return true;
}

void Sprite::Draw() {
    m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_cmdList->IASetVertexBuffers(0, 1, &m_vbView);
    m_cmdList->DrawInstanced(4, 1, 0, 0);
}
