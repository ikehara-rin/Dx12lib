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
    Microsoft::WRL::ComPtr<ID3D12Resource> texture;         // �e�N�X�`��
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;    // ���_�o�b�t�@
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;              // ���_�o�b�t�@�r���[
};
