// Renderer.cpp
#include "Renderer.h"
#include <stdexcept>    // ��O�p�i���s�� throw �p�j

Renderer::Renderer(ID3D12Device* device, ID3D12CommandQueue* commandQueue, IDXGISwapChain4* swapChain)
    : device_(device), commandQueue_(commandQueue), swapChain_(swapChain), rtvDescriptorSize_(0)
{
}

Renderer::~Renderer()
{
}

void Renderer::Initialize()
{
    CreateCommandAllocatorAndList();
    CreateRenderTargetView();
}

void Renderer::Render()
{
    // �R�}���h���X�g�����Z�b�g
    commandAllocator_->Reset();
    commandList_->Reset(commandAllocator_.Get(), nullptr);

    // ���݂̃o�b�N�o�b�t�@�C���f�b�N�X���擾
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    // �o�b�N�o�b�t�@��RTV�n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += backBufferIndex * rtvDescriptorSize_;

    // ���\�[�X�o���A�iPresent��RenderTarget�j
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = renderTargets_[backBufferIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList_->ResourceBarrier(1, &barrier);

    // �����_�[�^�[�Q�b�g��ݒ�
    commandList_->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // ��ʂ��N���A�i�w�i�F��ݒ�j
    FLOAT clearColor[] = { 0.2f, 0.4f, 0.6f, 1.0f }; // RGBA�i������ς���ƐF���ς��j
    commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // ���\�[�X�o���A�iRenderTarget��Present�j
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList_->ResourceBarrier(1, &barrier);

    // �R�}���h���X�g��Close����GPU�ɑ��鏀��
    commandList_->Close();

    // �R�}���h���X�g�����s
    ID3D12CommandList* cmdLists[] = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, cmdLists);

    // �o�b�N�o�b�t�@��O�ʂɕ\��
    swapChain_->Present(1, 0);
}

void Renderer::Finalize()
{
    // ComPtr�Ȃ̂Ŏ����ŉ������邪�A�����I�Ƀ��Z�b�g���ł���
    commandList_.Reset();
    commandAllocator_.Reset();
    swapChain_.Reset();
    commandQueue_.Reset();
    device_.Reset();
}

void Renderer::CreateCommandAllocatorAndList()
{
    // �R�}���h�A���P�[�^���R�}���h���X�g���g���������m��
    device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));

    // �R�}���h���X�g���`�施�߂𗭂߂�o�b�t�@
    device_->CreateCommandList(
        0,                              // �m�[�hID�i�V���O��GPU�Ȃ�0�j
        D3D12_COMMAND_LIST_TYPE_DIRECT, // �`��p�R�}���h���X�g
        commandAllocator_.Get(),         // �A���P�[�^���w��
        nullptr,                         // �ŏ��Ƀo�C���h����p�C�v���C���X�e�[�g�i���͂Ȃ��j
        IID_PPV_ARGS(&commandList_)
    );

    // �R�}���h���X�g�͍쐬����́u�I�[�v����ԁv�Ȃ̂ň�UClose����
    commandList_->Close();
}

void Renderer::CreateRenderTargetView()
{
    // ���o�b�t�@����SwapChain�쐬����2�ɂ����̂�2
    const UINT bufferCount = 2;

    // ��RTV�p�̃f�B�X�N���v�^�q�[�v���쐬����
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = bufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap_));

    // ���f�B�X�N���v�^1������̃T�C�Y���擾
    rtvDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // ���o�b�N�o�b�t�@�i�`��Ώہj�̃��\�[�X���擾����
    renderTargets_.resize(bufferCount);
    for (UINT i = 0; i < bufferCount; ++i)
    {
        swapChain_->GetBuffer(i, IID_PPV_ARGS(&renderTargets_[i]));

        // ��RTV���쐬���ăf�B�X�N���v�^�q�[�v�ɓo�^����
        D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += i * rtvDescriptorSize_;  // i�Ԗڂ̏ꏊ��

        device_->CreateRenderTargetView(renderTargets_[i].Get(), nullptr, handle);
    }
}
