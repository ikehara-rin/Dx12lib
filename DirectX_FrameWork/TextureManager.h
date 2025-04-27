#pragma once
#include <string>
#include <unordered_map>
#include <d3dx9.h> // D3DXライブラリ（画像ロード用）
#include "Renderer.h" // Rendererからデバイスをもらうため

// テクスチャ管理クラス
class TextureManager
{
public:
    // 画像を読み込む（filePath = ファイル名、keyName = 登録名）
    static bool LoadTexture(const std::string& filePath, const std::string& keyName);

    // 画像を描画する（keyNameで指定、座標(x,y)）
    static void DrawTexture(const std::string& keyName, float x, float y);

    // 全画像を解放する
    static void Shutdown();

private:
    // テクスチャを保存するマップ（キー：登録名、バリュー：テクスチャ本体）
    static std::unordered_map<std::string, LPDIRECT3DTEXTURE9> textures;
};


