// Dx12App.cpp
#include "Dx12App.h"
#include <stdexcept>    // ��O�p�i���s�� throw �p�j

Dx12App::Dx12App()
    : width_(0), height_(0)
{
}

Dx12App::~Dx12App()
{
}

bool Dx12App::Initialize(HWND hwnd, int width, int height)
{
    // �E�B���h�E�T�C�Y��ۑ�
    width_ = width;
    height_ = height;

    // �e�평�������������ԂɎ��s
    CreateDevice();
    CreateCommandQueue();
    CreateSwapChain(hwnd, width, height);
    CreateCommandAllocatorAndList();

    CreateRenderTargetView();
    return true;
}

// �Q�[�����W�b�N��A�j���[�V�����X�V�����������ɏ���
void Dx12App::Update()
{
    
    // ���͉����Ȃ��i��Œǉ��j
}

// �`�揈��
void Dx12App::Render()
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

void Dx12App::Finalize()
{
    // ComPtr�Ȃ̂Ŏ����ŉ������邪�A�����I�Ƀ��Z�b�g���ł���
    commandList_.Reset();
    commandAllocator_.Reset();
    swapChain_.Reset();
    commandQueue_.Reset();
    device_.Reset();
}

// ==========================
// �ȉ��A�����������֐�
// ==========================

void Dx12App::CreateDevice()
{
    // DXGI�t�@�N�g���[�쐬�iGPU�����擾������A�f�o�C�X�쐬�����j
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    // GPU�A�_�v�^�[�擾�i�����0�ԁ���ԗǂ�GPU�����j
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    factory->EnumAdapters1(0, &adapter);

    // DirectX12�f�o�C�X�쐬�iGPU�ɖ��߂𑗂邽�߂̃��C���I�u�W�F�N�g�j
    D3D12CreateDevice(
        adapter.Get(),
        D3D_FEATURE_LEVEL_11_0,   // �K�v�Œ���̋@�\���x���i12_0�ł�OK�j
        IID_PPV_ARGS(&device_)
    );
}

void Dx12App::CreateCommandQueue()
{
    // �R�}���h�L���[�ݒ�
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;  // ���ڕ`��R�}���h�𑗂����

    // �R�}���h�L���[�쐬
    device_->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue_));
}

void Dx12App::CreateSwapChain(HWND hwnd, int width, int height)
{
    // �X���b�v�`�F�C������ʂɊG��\�����邽�߂̃o�b�t�@
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;                       // �_�u���o�b�t�@�����O
    swapChainDesc.Width = width;                         // �E�B���h�E��
    swapChainDesc.Height = height;                       // �E�B���h�E����
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // �J���[�t�H�[�}�b�g�iRGBA8bit�j
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �`��p
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // ���_���ȃy�[�W�t���b�v����
    swapChainDesc.SampleDesc.Count = 1;                  // �}���`�T���v�����O�Ȃ�

    // �X���b�v�`�F�C���쐬�i�E�B���h�E�ɐڑ��j
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChainTemp;
    factory->CreateSwapChainForHwnd(
        commandQueue_.Get(),     // �R�}���h�L���[�Ɗ֘A�t��
        hwnd,                    // �E�B���h�E�n���h��
        &swapChainDesc,           // �ݒ���
        nullptr,                  // �t���X�N���[���ݒ�i���̓E�B���h�E�j
        nullptr,                  // �o�͐惂�j�^�i�����I���j
        &swapChainTemp            // �Ԃ�l
    );

    // IDXGISwapChain4�ɃL���X�g
    swapChainTemp.As(&swapChain_);
}

void Dx12App::CreateCommandAllocatorAndList()
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

void Dx12App::CreateRenderTargetView()
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