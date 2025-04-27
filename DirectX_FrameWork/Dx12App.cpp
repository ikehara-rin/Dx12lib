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

    return true;
}

void Dx12App::Update()
{
    // �Q�[�����W�b�N��A�j���[�V�����X�V�����������ɏ���
    // ���͉����Ȃ��i��Œǉ��j
}

void Dx12App::Render()
{
    // �`�揈��
    // ���͂܂���ʃN���A�����Ă��Ȃ��i���RenderTargetView�쐬���Ēǉ��\��j
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
