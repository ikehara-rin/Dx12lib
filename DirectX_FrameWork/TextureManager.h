#pragma once
#include <string>
#include <unordered_map>
#include <d3dx9.h> // D3DX���C�u�����i�摜���[�h�p�j
#include "Renderer.h" // Renderer����f�o�C�X�����炤����

// �e�N�X�`���Ǘ��N���X
class TextureManager
{
public:
    // �摜��ǂݍ��ށifilePath = �t�@�C�����AkeyName = �o�^���j
    static bool LoadTexture(const std::string& filePath, const std::string& keyName);

    // �摜��`�悷��ikeyName�Ŏw��A���W(x,y)�j
    static void DrawTexture(const std::string& keyName, float x, float y);

    // �S�摜���������
    static void Shutdown();

private:
    // �e�N�X�`����ۑ�����}�b�v�i�L�[�F�o�^���A�o�����[�F�e�N�X�`���{�́j
    static std::unordered_map<std::string, LPDIRECT3DTEXTURE9> textures;
};


