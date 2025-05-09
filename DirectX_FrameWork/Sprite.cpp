#include "Sprite.h"
#include "TextureLoader.h"  // �e�N�X�`���ǂݍ��ݕ⏕�i��ō쐬�j

using namespace DirectX;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT2 uv;
};

Sprite::Sprite() {}

Sprite::~Sprite() {}

bool Sprite::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const wchar_t* texturePath) {
    // �e�N�X�`���ǂݍ��݁i���F������TextureLoader�Łj
    if (!LoadTextureFromFile(device, commandList, texturePath, texture)) {
        return false;
    }

    // ���_�f�[�^�i��ʒ����ɉ摜1���j
    Vertex vertices[] = {
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
    };

    const UINT vertexBufferSize = sizeof(vertices);

    // ���_�o�b�t�@�쐬
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = vertexBufferSize;
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer));

    // ���_�f�[�^���R�s�[
    UINT8* pVertexDataBegin;
    D3D12_RANGE readRange = {};
    vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices, sizeof(vertices));
    vertexBuffer->Unmap(0, nullptr);

    // �r���[�ݒ�
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vertexBufferSize;

    return true;
}

void Sprite::Draw(ID3D12GraphicsCommandList* commandList) {
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

    // �ʏ�͂����Ńe�N�X�`����PSO��ݒ�i����͉��j
    commandList->DrawInstanced(4, 1, 0, 0);
}
