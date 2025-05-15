#include "SpriteRenderer.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <stdexcept>
#include <vector>
#include <d3dcompiler.h>

using namespace Microsoft::WRL;
using namespace DirectX;

SpriteRenderer::SpriteRenderer(Renderer* renderer)
    : m_pConstantBufferWO(nullptr),m_renderer(renderer) {
}

bool SpriteRenderer::Initialize()
{
    if (!CreateRootSignature()) return false;
    if (!CreatePipelineState()) return false;
    if (!CreateVertexBuffer()) return false;

    

    return true;
}

void SpriteRenderer::DrawCell(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
    float x, float y,
    float cellSize)
{

    assert(m_pConstantBufferWO != nullptr);
    assert(m_constantBuffer != nullptr);
    assert(commandList != nullptr);
    
    if (!commandList) return;
    
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

bool SpriteRenderer::CreateRootSignature() {
    /*ComPtr<ID3D12Device> device = m_renderer->GetDevice();*/

    D3D12_DESCRIPTOR_RANGE1 range = {};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.NumDescriptors = 1;
    range.BaseShaderRegister = 0;
    range.RegisterSpace = 0;
    range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER1 rootParam = {};
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParam.DescriptorTable.NumDescriptorRanges = 1;
    rootParam.DescriptorTable.pDescriptorRanges = &range;
    rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSignatureDesc.Desc_1_1.NumParameters = 1;
    rootSignatureDesc.Desc_1_1.pParameters = &rootParam;
    rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
    rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
    rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signatureBlob, errorBlob;
    D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signatureBlob, &errorBlob);
    return SUCCEEDED(m_renderer->GetDevice()->CreateRootSignature(
        0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)));
}

bool SpriteRenderer::CreatePipelineState() {
    // 仮の頂点シェーダとピクセルシェーダのバイナリを用意しておく必要があります。
     // ここでは省略。実際には HLSL をコンパイルしたバイナリを埋め込む。
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;

    ComPtr<ID3DBlob> errorBlob;
    HRESULT bl = D3DCompileFromFile(
        L"SpriteVS.hlsl", nullptr, nullptr,
        "main", "vs_5_0", 0, 0,
        &vsBlob, &errorBlob
    );

    if (FAILED(bl)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());  // エラー内容をデバッグ出力
        }
        return FAILED(bl);
    };
    D3DCompileFromFile(L"SpritePS.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);
     // 例:
    /*extern const BYTE g_vs_main[];
    extern const SIZE_T g_vs_main_size;
    extern const BYTE g_ps_main[];
    extern const SIZE_T g_ps_main_size;*/

    D3D12_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    
    // 入力レイアウト定義
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // ラスタライザーステート
    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.ForcedSampleCount = 0;
    rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // ブレンドステート
    D3D12_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
        FALSE, FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    blendDesc.RenderTarget[0] = defaultRenderTargetBlendDesc;

    // 深度ステンシルステート（未使用）
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    // PSOの全体設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    psoDesc.RasterizerState = rasterizerDesc;
    psoDesc.BlendState = blendDesc;
    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    HRESULT hr = m_renderer->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
    return SUCCEEDED(hr);
}

void SpriteRenderer::DrawSprite(ID3D12Resource* texture, float x, float y, float width, float height) {
    ComPtr<ID3D12GraphicsCommandList> cmdList = m_renderer->GetCommandList();

    cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
    cmdList->SetPipelineState(m_pipelineState.Get());
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);

    ID3D12DescriptorHeap* heaps[] = { m_renderer->GetSRVHeap() };
    cmdList->SetDescriptorHeaps(1, heaps);
    cmdList->SetGraphicsRootDescriptorTable(0, m_renderer->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart());

    // viewport transform や world transform は省略（必要に応じて追加）
    cmdList->DrawInstanced(4, 1, 0, 0);
}

bool SpriteRenderer::CreateVertexBuffer() {
    struct Vertex { XMFLOAT3 pos; XMFLOAT2 uv; };
    Vertex vertices[] =
    {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},
    };

    const UINT vertexBufferSize = sizeof(vertices);

    ComPtr<ID3D12Device> device = m_renderer->GetDevice();

    ID3D12DescriptorHeap* srvHeap = m_renderer->GetSRVHeap();

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
    /*const UINT cbSize = (sizeof(XMMATRIX) + 255) & ~255;
    bufferDesc.Width = cbSize;*/

    // 定数バッファ作成
    D3D12_RESOURCE_DESC cbDesc = {};
    cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    cbDesc.Width = (sizeof(XMMATRIX) + 255) & ~255; // 256バイトアライン
    cbDesc.Height = 1;
    cbDesc.DepthOrArraySize = 1;
    cbDesc.MipLevels = 1;
    cbDesc.Format = DXGI_FORMAT_UNKNOWN;
    cbDesc.SampleDesc.Count = 1;
    cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_HEAP_PROPERTIES cbheapProps = {};
    cbheapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    hr = device->CreateCommittedResource(
        &cbheapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer)
    );
    if (FAILED(hr)) throw std::runtime_error("Constant buffer creation failed");

    // マップ
    m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstantBufferWO));
   /* ZeroMemory(m_pConstantBufferWO, cbSize);*/

    // SRVディスクリプタハンドル
    m_srvHandleGPU = srvHeap->GetGPUDescriptorHandleForHeapStart();
    return true;
}