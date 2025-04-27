#include "TextureManager.h"

// 静的メンバの実体
std::unordered_map<std::string, LPDIRECT3DTEXTURE9> TextureManager::textures;

bool TextureManager::LoadTexture(const std::string& filePath, const std::string& keyName)
{
    // テクスチャ一時ポインタ
    LPDIRECT3DTEXTURE9 texture = nullptr;

    // 画像をファイルからロード
    if (FAILED(D3DXCreateTextureFromFile(Renderer::GetDevice(), filePath.c_str(), &texture)))
    {
        return false; // ロード失敗
    }

    // マップに保存
    textures[keyName] = texture;

    return true;
}

void TextureManager::DrawTexture(const std::string& keyName, float x, float y)
{
    auto it = textures.find(keyName);
    if (it == textures.end())
    {
        return; // 登録されてないキーなら描画しない
    }

    // スプライト描画のためにD3DXスプライトを使う
    static LPD3DXSPRITE sprite = nullptr;
    if (!sprite)
    {
        // スプライトオブジェクト作成
        D3DXCreateSprite(Renderer::GetDevice(), &sprite);
    }

    // 描画開始
    sprite->Begin(D3DXSPRITE_ALPHABLEND);

    // 描画位置を設定
    D3DXVECTOR3 position(x, y, 0);

    // テクスチャ描画
    sprite->Draw(it->second, nullptr, nullptr, &position, D3DCOLOR_XRGB(255, 255, 255));

    // 描画終了
    sprite->End();
}

void TextureManager::Shutdown()
{
    // 保存されているすべてのテクスチャを解放
    for (auto& tex : textures)
    {
        if (tex.second)
        {
            tex.second->Release();
        }
    }
    textures.clear(); // マップを空にする
}
