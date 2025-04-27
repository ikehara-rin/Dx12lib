// Dx12App.h
#pragma once

// �K�v��Windows�w�b�_�[�ƁADirectX12�p�w�b�_�[
#include <windows.h>
#include <wrl.h>        // ComPtr���g�����߁i�Q�ƃJ�E���g�����Ǘ��X�}�[�g�|�C���^�j
#include <d3d12.h>      // DirectX12�{��
#include <dxgi1_6.h>    // DXGI�F�O���t�B�b�N�X�C���^�[�t�F�[�X�Ǘ�

// DirectX12��{�A�v���P�[�V�����N���X
class Dx12App
{
public:
    Dx12App();
    ~Dx12App();

    // ����������
    bool Initialize(HWND hwnd, int width, int height);

    // ���t���[���X�V����
    void Update();

    // ���t���[���`�揈��
    void Render();

    // �I�������i���\�[�X����j
    void Finalize();

private:
    // �����I�ȏ������֐��i�e�X�e�b�v�𕪗��j
    void CreateDevice();                  // �f�o�C�X�쐬
    void CreateCommandQueue();             // �R�}���h�L���[�쐬
    void CreateSwapChain(HWND hwnd, int width, int height);  // �X���b�v�`�F�C���쐬
    void CreateCommandAllocatorAndList();  // �R�}���h�A���P�[�^�{�R�}���h���X�g�쐬

private:
    // DirectX12�I�u�W�F�N�g�Ǘ�
    Microsoft::WRL::ComPtr<ID3D12Device> device_;                   // GPU�Ƃ��Ƃ肷�鑋��
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;        // GPU�ɖ��߂𑗂邽�߂̃L���[
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;              // �\���p�o�b�N�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_; // �R�}���h�o�b�t�@�������m��
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;   // �`��R�}���h���l�߂郊�X�g

    // �E�B���h�E�T�C�Y
    int width_;
    int height_;
};
