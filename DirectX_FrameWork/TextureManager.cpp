#include "TextureManager.h"

// �ÓI�����o�̎���
std::unordered_map<std::string, LPDIRECT3DTEXTURE9> TextureManager::textures;

bool TextureManager::LoadTexture(const std::string& filePath, const std::string& keyName)
{
    // �e�N�X�`���ꎞ�|�C���^
    LPDIRECT3DTEXTURE9 texture = nullptr;

    // �摜���t�@�C�����烍�[�h
    if (FAILED(D3DXCreateTextureFromFile(Renderer::GetDevice(), filePath.c_str(), &texture)))
    {
        return false; // ���[�h���s
    }

    // �}�b�v�ɕۑ�
    textures[keyName] = texture;

    return true;
}

void TextureManager::DrawTexture(const std::string& keyName, float x, float y)
{
    auto it = textures.find(keyName);
    if (it == textures.end())
    {
        return; // �o�^����ĂȂ��L�[�Ȃ�`�悵�Ȃ�
    }

    // �X�v���C�g�`��̂��߂�D3DX�X�v���C�g���g��
    static LPD3DXSPRITE sprite = nullptr;
    if (!sprite)
    {
        // �X�v���C�g�I�u�W�F�N�g�쐬
        D3DXCreateSprite(Renderer::GetDevice(), &sprite);
    }

    // �`��J�n
    sprite->Begin(D3DXSPRITE_ALPHABLEND);

    // �`��ʒu��ݒ�
    D3DXVECTOR3 position(x, y, 0);

    // �e�N�X�`���`��
    sprite->Draw(it->second, nullptr, nullptr, &position, D3DCOLOR_XRGB(255, 255, 255));

    // �`��I��
    sprite->End();
}

void TextureManager::Shutdown()
{
    // �ۑ�����Ă��邷�ׂẴe�N�X�`�������
    for (auto& tex : textures)
    {
        if (tex.second)
        {
            tex.second->Release();
        }
    }
    textures.clear(); // �}�b�v����ɂ���
}
