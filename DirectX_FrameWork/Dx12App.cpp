// Dx12App.cpp
#include "Dx12App.h"
#include "Renderer.h"
#include <stdexcept>    // ��O�p�i���s�� throw �p�j

Dx12App::Dx12App()
    : width_(0), height_(0), renderer_(nullptr)
{
}

Dx12App::~Dx12App()
{
    if (renderer_)
    {
        delete renderer_;
    }
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

    // Renderer��������
    renderer_ = new Renderer(device_.Get(), commandQueue_.Get(), swapChain_.Get());
    renderer_->Initialize();

    return true;
}

void Dx12App::Update()
{
    // �Q�[�����W�b�N��A�j���[�V�����X�V�����i��Œǉ��j
}

void Dx12App::Render()
{
    // �����_�����O����
    renderer_->Render();
}

void Dx12App::Finalize()
{
    // ���\�[�X���
    renderer_->Finalize();
    delete renderer_;
    renderer_ = nullptr;
}

// ==========================
// �ȉ��A�����������֐�
// ==========================
