#include "TextureLoader.h"
#include <vector>
#include <iostream>

using Microsoft::WRL::ComPtr;

TextureLoader::TextureLoader(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
    : m_device(device), m_commandList(commandList)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_wicFactory));
}

TextureLoader::~TextureLoader()
{
    CoUninitialize();
}

HRESULT TextureLoader::LoadTexture(
    const std::wstring& fileName,
    ID3D12Resource** texture,
    ID3D12Resource** uploadHeap,
    UINT* width,
    UINT* height)
{
    // 画像をWICから読み込む
    ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(fileName.c_str(), nullptr, GENERIC_READ,
        WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr)) return hr;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return hr;

    hr = frame->GetSize(width, height);
    if (FAILED(hr)) return hr;

    // 32bit RGBA に変換
    ComPtr<IWICFormatConverter> converter;
    hr = m_wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return hr;

    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return hr;

    std::vector<BYTE> imageData((*width) * (*height) * 4);
    hr = converter->CopyPixels(nullptr, (*width) * 4, static_cast<UINT>(imageData.size()), imageData.data());
    if (FAILED(hr)) return hr;

    // テクスチャリソース作成
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = *width;
    texDesc.Height = *height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    hr = m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(texture));
    if (FAILED(hr)) return hr;

    // アップロードヒープ作成
    UINT64 imageSize = static_cast<UINT64>(imageData.size());

    D3D12_RESOURCE_DESC uploadDesc = {};
    uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadDesc.Width = imageSize;
    uploadDesc.Height = 1;
    uploadDesc.DepthOrArraySize = 1;
    uploadDesc.MipLevels = 1;
    uploadDesc.SampleDesc.Count = 1;
    uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    hr = m_device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadHeap));
    if (FAILED(hr)) return hr;

    // マップしてデータをコピー
    void* pData = nullptr;
    hr = (*uploadHeap)->Map(0, nullptr, &pData);
    if (FAILED(hr)) return hr;

    memcpy(pData, imageData.data(), imageSize);
    (*uploadHeap)->Unmap(0, nullptr);

    // コマンドでテクスチャ転送
    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource = *texture;
    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource = *uploadHeap;
    src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    src.PlacedFootprint.Footprint.Width = *width;
    src.PlacedFootprint.Footprint.Height = *height;
    src.PlacedFootprint.Footprint.Depth = 1;
    src.PlacedFootprint.Footprint.RowPitch = ((*width * 4 + 255) & ~255);

    m_commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    return S_OK;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureLoader::CreateSRV(
    ID3D12Resource* texture,
    ID3D12DescriptorHeap* descriptorHeap,
    UINT descriptorIndex)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    UINT descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    handle.ptr += descriptorIndex * descriptorSize;

    m_device->CreateShaderResourceView(texture, &srvDesc, handle);
    return handle;
}

