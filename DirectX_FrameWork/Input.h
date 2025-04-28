#pragma once
#include <Windows.h>

// 入力を管理するクラス
class Input
{
public:
    // キーの状態
    bool m_keys[256] = {};

    // 初期化
    void Initialize();

    // キーを押したときに呼ぶ
    void KeyDown(WPARAM key);

    // キーを離したときに呼ぶ
    void KeyUp(WPARAM key);

    // キーが押されているかを判定する
    bool IsKeyDown(UINT keyCode) const;
};
