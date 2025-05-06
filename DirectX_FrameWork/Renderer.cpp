#include "Renderer.h"
#include <stdexcept>

Renderer::Renderer()
    : m_fenceValue(0), 
    m_frameIndex(0),
    m_fenceEvent(nullptr),
    m_rtvDescriptorSize(0)
{
    
}

Renderer::~Renderer()
{
    Release();
    
}

bool Renderer::Initialize(HWND hWnd)
{
    if (!InitD3D12(hWnd))
        return false;
    return true;
}

void Renderer::BeginRender() 
{
    // �R�}���h�A���P�[�^�ƃR�}���h���X�g�����Z�b�g
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);

    // �o�b�N�o�b�t�@�̃��\�[�X�o���A��Present �� RenderTarget�ɑJ��
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    // �����_�[�^�[�Q�b�g�r���[��ݒ�
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.ptr += m_frameIndex * m_rtvDescriptorSize;
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // �w�i�F�ŃN���A
    const float clearColor[] = { 0.2f, 0.3f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void Renderer::EndRender() {
    // �o�b�N�o�b�t�@�������_�[�^�[�Q�b�g �� Present�ɑJ��
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    // �R�}���h���X�g�����
    m_commandList->Close();

    // �R�}���h���X�g�����s
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // �X���b�v�`�F�C���ŉ�ʂɕ\��
    m_swapChain->Present(1, 0);

    // �t���[������
    const UINT64 currentFenceValue = m_fenceValue;
    m_commandQueue->Signal(m_fence.Get(), currentFenceValue);
    m_fenceValue++;

    if (m_fence->GetCompletedValue() < currentFenceValue) {
        m_fence->SetEventOnCompletion(currentFenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::Release()
{
    // GPU�����ׂẴR�}���h����������܂őҋ@
    if (m_commandQueue && m_fence && m_fenceEvent)
    {
        m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
        m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
        m_fenceValue++;
    }

    // �t�F���X�C�x���g�����
    if (m_fenceEvent)
    {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }

    // ���Ԃɉ���iComPtr�Ȃ̂� Release�͕s�v�Bnullptr�ň��S�ɏ�����j
    m_commandList.Reset();
    m_commandAllocator.Reset();
    m_fence.Reset();
    m_rtvHeap.Reset();
    for (int i = 0; i < 2; ++i)
    {
        m_renderTargets[i].Reset();
    }
    m_swapChain.Reset();
    m_commandQueue.Reset();
    m_device.Reset();
}
/**********************************/
//  �ȉ������֐�
/**********************************/

bool Renderer::InitD3D12(HWND hWnd)
{
    UINT dxgiFactoryFlags = 0;

    ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));

    // �f�o�C�X�쐬
    D3D12CreateDevice(
        nullptr, // �f�t�H���g�A�_�v�^�[
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&m_device)
    );

    // �R�}���h�L���[�쐬
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

    // �X���b�v�`�F�C���쐬
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Width = 800;
    swapChainDesc.Height = 600;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),
        hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    );
    swapChain.As(&m_swapChain);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // �f�B�X�N���v�^�q�[�v�쐬
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < 2; i++)
    {
        m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        // ���̃n���h���ʒu�ɃI�t�Z�b�g
        rtvHandle.ptr += m_rtvDescriptorSize;
    }

    // �R�}���h�A���P�[�^���R�}���h���X�g�쐬
    m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
    m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
    m_commandList->Close();

    // �t�F���X�쐬
    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    return true;
}