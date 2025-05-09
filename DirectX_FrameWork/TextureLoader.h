#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <cassert>

#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <wincodec.h>

class TextureLoader
{
public:
    TextureLoader(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    ~TextureLoader();

    HRESULT LoadTexture(
        const std::wstring& fileName,
        ID3D12Resource** texture,
        ID3D12Resource** uploadHeap,
        UINT* width,
        UINT* height
    );

    D3D12_CPU_DESCRIPTOR_HANDLE CreateSRV(
        ID3D12Resource* texture,
        ID3D12DescriptorHeap* descriptorHeap,
        UINT descriptorIndex
    );

private:
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
};

inline void UpdateSubresourcesSimple(
    ID3D12GraphicsCommandList* cmdList,
    ID3D12Resource* dstResource,
    ID3D12Resource* uploadResource,
    const void* data,
    UINT64 dataSize,
    UINT width,
    UINT height,
    UINT rowPitch)
{
    // �A�b�v���[�h�q�[�v���}�b�v
    void* mappedData = nullptr;
    D3D12_RANGE readRange = { 0, 0 }; // �ǂݎ��͂��Ȃ�
    HRESULT hr = uploadResource->Map(0, &readRange, &mappedData);
    assert(SUCCEEDED(hr));

    // 1�s���R�s�[�i�O�̂���rowPitch�ŏ����j
    uint8_t* dst = reinterpret_cast<uint8_t*>(mappedData);
    const uint8_t* src = reinterpret_cast<const uint8_t*>(data);
    for (UINT y = 0; y < height; ++y) {
        memcpy(dst + y * rowPitch, src + y * rowPitch, rowPitch);
    }

    uploadResource->Unmap(0, nullptr);

    // �R�s�[��̃T�u���\�[�X�w��
    D3D12_TEXTURE_COPY_LOCATION dstLoc = {};
    dstLoc.pResource = dstResource;
    dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLoc.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
    srcLoc.pResource = uploadResource;
    srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLoc.PlacedFootprint.Offset = 0;
    srcLoc.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srcLoc.PlacedFootprint.Footprint.Width = width;
    srcLoc.PlacedFootprint.Footprint.Height = height;
    srcLoc.PlacedFootprint.Footprint.Depth = 1;
    srcLoc.PlacedFootprint.Footprint.RowPitch = rowPitch;

    // �R�s�[�R�}���h�𔭍s
    cmdList->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);

    // �J�ځiCOPY_DEST �� PIXEL_SHADER_RESOURCE �Ȃǁj���R�}���h���X�g�̊O�ł��
}