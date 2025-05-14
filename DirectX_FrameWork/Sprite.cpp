#include "Sprite.h"
#include <wincodec.h>
#include <vector>
#include <stdexcept>

using namespace Microsoft::WRL;

struct Vertex {
    float pos[3];
    float uv[2];
};

Sprite::Sprite() : renderer(nullptr), textureWidth(0), textureHeight(0) {}

Sprite::~Sprite() {}

bool Sprite::Load(Renderer* renderer, const std::wstring& filePath) {
    this->renderer = renderer;

    if (!CreateTextureFromFile(filePath)) return false;
    if (!CreateVertexBuffer()) return false;

    return true;
}

bool Sprite::CreateTextureFromFile(const std::wstring& filePath) {
    // WIC (Windows Imaging Component) を使用して画像を読み込む
    IWICImagingFactory* wicFactory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) return false;

    hr = wicFactory->CreateDecoderFromFilename(
        filePath.c_str(), nullptr, GENERIC_READ,
        WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) return false;

    decoder->GetFrame(0, &frame);

    wicFactory->CreateFormatConverter(&converter);
    converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);

    UINT width, height;
    converter->GetSize(&width, &height);
    textureWidth = width;
    textureHeight = height;

    std::vector<UINT8> imageData(width * height * 4);
    converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(imageData.size()), imageData.data());

    // テクスチャ作成
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    hr = renderer->GetDevice()->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&texture));
    if (FAILED(hr)) return false;

    // アップロードバッファを作成してコピー
    const UINT uploadPitch = (width * 4 + 255) & ~255;
    const UINT uploadSize = uploadPitch * height;

    ComPtr<ID3D12Resource> uploadBuffer;
    D3D12_HEAP_PROPERTIES uploadProps = {};
    uploadProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC uploadDesc = {};
    uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadDesc.Width = uploadSize;
    uploadDesc.Height = 1;
    uploadDesc.DepthOrArraySize = 1;
    uploadDesc.MipLevels = 1;
    uploadDesc.SampleDesc.Count = 1;
    uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    hr = renderer->GetDevice()->CreateCommittedResource(
        &uploadProps, D3D12_HEAP_FLAG_NONE, &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer));
    if (FAILED(hr)) return false;

    // メモリコピー
    UINT8* pData = nullptr;
    uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
    for (UINT y = 0; y < height; ++y) {
        memcpy(pData + y * uploadPitch, imageData.data() + y * width * 4, width * 4);
    }
    uploadBuffer->Unmap(0, nullptr);

    // コピーコマンド発行
    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource = texture.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource = uploadBuffer.Get();
    src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint.Offset = 0;
    src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    src.PlacedFootprint.Footprint.Width = width;
    src.PlacedFootprint.Footprint.Height = height;
    src.PlacedFootprint.Footprint.Depth = 1;
    src.PlacedFootprint.Footprint.RowPitch = uploadPitch;

    auto cmdList = renderer->GetCommandList();
    cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = texture.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    cmdList->ResourceBarrier(1, &barrier);

    // TODO: SRV作成処理が別途必要（Rendererに委譲するか設計による）

    converter->Release();
    frame->Release();
    decoder->Release();
    wicFactory->Release();

    return true;
}

bool Sprite::CreateVertexBuffer() {
    // 矩形の2トライアングル（6頂点）
    Vertex vertices[6] = {
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{static_cast<float>(textureWidth), 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.0f, static_cast<float>(textureHeight), 0.0f}, {0.0f, 1.0f}},

        {{0.0f, static_cast<float>(textureHeight), 0.0f}, {0.0f, 1.0f}},
        {{static_cast<float>(textureWidth), 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{static_cast<float>(textureWidth), static_cast<float>(textureHeight), 0.0f}, {1.0f, 1.0f}},
    };

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC vbDesc = {};
    vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vbDesc.Width = sizeof(vertices);
    vbDesc.Height = 1;
    vbDesc.DepthOrArraySize = 1;
    vbDesc.MipLevels = 1;
    vbDesc.SampleDesc.Count = 1;
    vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = renderer->GetDevice()->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &vbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));
    if (FAILED(hr)) return false;

    void* mappedData = nullptr;
    vertexBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, vertices, sizeof(vertices));
    vertexBuffer->Unmap(0, nullptr);

    vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(Vertex);
    vbView.SizeInBytes = sizeof(vertices);

    return true;
}

void Sprite::Draw(float x, float y) {
    auto cmdList = renderer->GetCommandList();

    // ビューポート変換などの行列はここでは簡略化
    cmdList->IASetVertexBuffers(0, 1, &vbView);
    cmdList->DrawInstanced(6, 1, 0, 0);
}
